<?php

/**
 * Class default_apihub_handler
 */
class apihubHandlerDrupal extends apihubHandler {
  /**
   * @var string
   */
  protected $auth = '';

  /**
   * @var string
   */
  protected $url = '';

  /**
   * {@inheritdoc}
   */
  public function __construct(array $settings = array()) {
    if (isset($settings['auth'])) {
      $this->auth = $settings['auth'];
    }

    if (isset($settings['url'])) {
      $this->url = $settings['url'];
    }
  }

  /**
   * @return array
   */
  public function settingsForm() {
    return array(
      'url'  => array(
        'type'        => 'text',
        'name'        => t('URL'),
        'description' => t('Server URL, leave blank to use API default'),
      ),
      'auth' => array(
        'type' => 'text',
        'name' => t('Authentication'),
      ),
    );
  }

  /**
   * {@inheritdoc}
   */
  public function execute($url, array $parameters = array(), $method = 'GET', array $options = array()) {
    if (!empty($this->auth)) {
      $parameters = array_merge($parameters, drupal_get_query_array($this->auth));
    }

    $url     = url($url, array('query' => $parameters));
    $request = drupal_http_request($url, array(
      'method' => $method,
    ));

    // Debug request object.
    if (isset($options['debug']) && module_exists('devel')) {
      dpm($request, t('Request'));
    }

    // Standardise data.
    $data = $request->data;
    switch (TRUE) {
      case strstr($request->headers['content-type'], 'json'):
        $data = json_decode($data);
        break;

      case strstr($request->headers['content-type'], 'xml'):
        // @TODO - Standardise @attributes?.
        $xml  = simplexml_load_string($data);
        $json = json_encode($xml);
        $data = json_decode($json, TRUE);
        break;
    }

    return $data;
  }

  /**
   * {@inheritdoc}
   */
  public function getUrl($url, $path) {
    if (!empty($this->url)) {
      $url = $this->url;
    }

    return $url . $path;
  }
}
