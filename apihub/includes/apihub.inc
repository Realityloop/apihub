<?php
/**
 * @file
 * API Hub module integration.
 */

/**
 * Implements hook_apihub_handler_info().
 */
function apihub_apihub_handler_info() {
  return array(
    'drupal' => array(
      'name'     => t('Drupal'),
      'class'    => 'drupal_apihub_handler',
      'settings' => array(
        'auth' => array(
          'type' => 'text',
          'name' => t('Authentication'),
        ),
      ),
    ),
  );
}

/**
 * Class default_apihub_handler
 */
class drupal_apihub_handler extends apihub_handler {
  protected $auth = NULL;

  /**
   * @param array $args
   */
  function __construct($args = array()) {
    if (isset($args['auth'])) {
      $this->auth = $args['auth'];
    }
  }

  /**
   * @param       $method
   * @param       $url
   * @param array $params
   *
   * @return mixed
   */
  function execute($method, $url, $params = array()) {
    $data = '';
    if ($method == 'GET') {
      $params += drupal_get_query_array($this->auth);
    }
    else {
      $data   = drupal_http_build_query($params);
      $params = drupal_get_query_array($this->auth);
    }
    $url = url($url, array('query' => $params));

    $request = drupal_http_request($url, array(
      'method' => $method,
      'data'   => $data,
    ));

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
}
