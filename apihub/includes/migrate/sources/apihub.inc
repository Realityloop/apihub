<?php
/**
 * @file
 * Define a MigrateSource for importing from API Hub resources.
 */

/**
 * Implementation of MigrateSource, to handle imports from API Hub resource.
 */
class MigrateSourceAPIHub extends MigrateSource {
  /**
   * API Hub handler and settings.
   *
   * @var array
   */
  protected $handler = array();

  /**
   * Query offset for paged queries.
   *
   * @var int
   */
  protected $offset = 0;

  /**
   * API Hub request parameters.
   *
   * @var array
   */
  protected $parameters = array();

  /**
   * API Hub resource.
   *
   * @var array
   */
  protected $resource = array();

  /**
   * Query results.
   *
   * @var array
   */
  protected $results = array();

  /**
   * Query results iterator operation.
   *
   * @var string
   */
  protected $iteratorOp = 'current';

  /**
   * Simple initialization.
   *
   * @param array $options
   *  Options applied to this source.
   */
  public function __construct($resource, array $handler = array(), array $parameters = array(), array $options = array()) {
    parent::__construct($options);

    ctools_include('export');
    $resource = ctools_export_crud_load('apihub_resources', $resource);
    if (!is_null($resource)) {
      $this->resource = $resource;
    }
    $this->handler    = $handler;
    $this->parameters = $parameters;
  }

  /**
   * Return a string representing the source query.
   *
   * @return string
   */
  public function __toString() {
    // @TODO - Return the URL(s) being queried.
    return $this->resource->name;
  }

  /**
   * Returns a list of fields available to be mapped from the source query.
   *
   * @return array
   *  Keys: machine names of the fields (to be passed to addFieldMapping)
   *  Values: Human-friendly descriptions of the fields.
   */
  public function fields() {
    $fields = array();

    return $fields;
  }

  /**
   * Return a count of all available source records.
   */
  public function computeCount() {
    $results = $this->getResults();

    // If a pager total count element is defined, return it's value.
    if (isset($this->resource->settings['apihub']['pager']['total']) && isset($results->{$this->resource->settings['apihub']['pager']['total']})) {
      return $results->{$this->resource->settings['apihub']['pager']['total']};
    }

    // If a root element is defined, adjust the results for the count.
    if (isset($this->resource->settings['apihub']['root']) && isset($results->{$this->resource->settings['apihub']['root']})) {
      $results = $results->{$this->resource->settings['apihub']['root']};
    }

    return count($results);
  }

  /**
   * Return the results of an API Hub resource.
   */
  public function getResults() {
    if (empty($this->results)) {
      // Process the request.
      $request = new apihub_request($this->resource->api, $this->handler['handler'], $this->handler['settings']);
      if (isset($this->resource->settings['apihub']['pager']['offset'])) {
        $this->parameters[$this->resource->settings['apihub']['pager']['offset']] = $this->offset;
      }
      $this->results = $request->execute($this->resource->method, $this->resource->path, $this->parameters);
    }

    return $this->results;
  }

  /**
   * Implementation of MigrateSource::performRewind().
   *
   * @return void
   */
  public function performRewind() {
    $this->results = $this->currentRow = array();
    $this->getResults(0);
  }

  /**
   * Implementation of MigrateSource::getNextRow().
   *
   * @return null|object
   */
  public function getNextRow() {
    $results =& $this->results;

    // If a root element is defined, adjust the results for the count.
    if (isset($this->resource->settings['apihub']['root']) && isset($this->results->{$this->resource->settings['apihub']['root']})) {
      $results =& $this->results->{$this->resource->settings['apihub']['root']};
    }

    $row              = $this->iteratorOp == 'current' ? current($results) : next($results);
    $this->iteratorOp = 'next';

    // If we are out of rows, we are either out of data or we need to page.
    if (!$row) {
      if (isset($this->resource->settings['apihub']['pager']['offset']) && isset($this->results->{$this->resource->settings['apihub']['pager']['offset']})) {
        $offset = $this->offset + $this->resource->settings['apihub']['pager']['offset'] + count($results);

        if ($offset < $this->computeCount()) {
          $this->offset     = $offset;
          $this->results    = array();
          $this->iteratorOp = 'current';
          $this->getResults();

          return $this->getNextRow();
        }
      }

      return NULL;
    }

    $this->currentRow = $row;
    $this->flatten($this->currentRow);

    return $this->currentRow;
  }

  /**
   * Flatten the current row structure.
   *
   * @param       $elements
   * @param array $parents
   */
  function flatten($elements, $parents = array()) {
    foreach ($elements as $key => $element) {
      if (is_object($element)) {
        $parents[] = $key;
        $this->flatten($element, $parents);
        array_pop($parents);
        unset($elements->$key);
      }
      elseif (!empty($parents)) {
        $this->currentRow->{implode(':', $parents) . ":{$key}"} = $element;
      }
    }
  }
}
