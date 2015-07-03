<?php

/**
 * Class apihubMethod
 */
class apihubMethod {
  /**
   * @var null
   */
  protected $api = NULL;

  /**
   * @var array
   */
  protected $requestParameters = array();

  /**
   * @param $key
   * @param $value
   */
  public function __set($key, $value) {
    if ($key == 'api' && is_string($value)) {
      ctools_include('export');
      $value = ctools_export_crud_load('apihub_apis', $value);
    }
    $this->{$key} = $value;
  }

  /**
   * @param $key
   *
   * @return null
   */
  public function __get($key) {
    if (isset($this->{$key})) {
      return $this->{$key};
    }

    return NULL;
  }

  /**
   * @return string
   */
  public function getUrl() {
    $url  = $this->api->url;
    $path = $this->path;

    // Substitute parameters in URL and path.
    $remove = array();
    foreach (array('url', 'path') as $part) {
      foreach ($this->requestParameters as $key => $value) {
        foreach (array(":{$key}", "[{$key}]") as $needle) {
          if (strstr($$part, $needle)) {
            $$part        = str_replace($needle, $value, $$part);
            $remove[$key] = TRUE;
            break;
          }
        }
      }
    }

    // Remove parameters used in URL substitution.
    foreach (array_keys($remove) as $key) {
      unset($this->requestParameters[$key]);
    }

    if (is_object($this->handler)) {
      return $this->handler->getUrl($url, $path);
    }

    return $url . $path;
  }

  /**
   * @param array $handler
   */
  public function setHandler(array $handler = array()) {
    if (isset($handler['class']) && class_exists($handler['class'])) {
      $settings      = is_array($handler['settings']) ? $handler['settings'] : array();
      $this->handler = new $handler['class']($settings);
    }
  }

  /**
   * @param array $parameters
   */
  public function setParameters(array $parameters = array()) {
    // Cleanup parameters.
    // @TODO - Ensure required parameters are provided and not empty.
    $parameters = is_null($parameters) ? array() : $parameters;
    foreach ($parameters as $key => $value) {
      if ((empty($parameters[$key]) && !is_numeric($parameters[$key])) || is_null($parameters[$key])) {
        unset($parameters[$key]);
      }
    }

    $this->requestParameters = $parameters;
  }

  /**
   * @param array $parameters
   * @param array $options
   */
  public function execute(array $parameters = array(), array $handler = array(), array $options = array()) {
    if (!empty($parameters)) {
      $this->setParameters($parameters);
    }

    // Setup the handler.
    if (!empty($handler)) {
      $this->setHandler($handler, $parameters);
    }

    // Ensure that the handler is setup correctly.
    if (!is_object($this->handler)) {
      throw new Exception('API Hub method handler is missing.');
    }

    return $this->handler->execute($this->getUrl(), $this->requestParameters, $this->http_method, $options);
  }
}
