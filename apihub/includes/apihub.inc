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
        'url'  => array(
          'type'        => 'text',
          'name'        => t('URL'),
          'description' => t('Server URL, leave blank to use API default'),
        ),
        'auth' => array(
          'type' => 'text',
          'name' => t('Authentication'),
        ),
      ),
    ),
  );
}

/**
 * Implement hook_apihub_pre_execute().
 */
function apihub_apihub_pre_execute($resource, $apihub) {
  // @TODO - Also check if the whole API is offline.
  if (isset($resource->settings['apihub']['offline']) && $resource->settings['apihub']['offline']) {
    drupal_set_message(t('Resource "@resource" is in offline mode.', array(
      '@resource' => $resource->admin_title,
    )), 'status', FALSE);

    $apihub->setHandler(new offline_apihub_handler());
  }
}

/**
 * Class default_apihub_handler
 */
class drupal_apihub_handler extends apihub_handler {
  protected $auth = NULL;
  protected $url = NULL;

  /**
   * @param array $args
   */
  function __construct($args = array()) {
    if (isset($args['auth'])) {
      $this->auth = $args['auth'];
    }

    if (isset($args['url'])) {
      $this->url = $args['url'];
    }
  }

  /**
   * @param       $method
   * @param       $url
   * @param array $params
   * @param       $resource
   *
   * @return mixed
   */
  function execute($method, $url, $params = array(), $resource) {
    // Override API url with handler provided URL.
    if (!empty($this->url)) {
      ctools_include('export');
      $api = ctools_export_crud_load('apihub_apis', $resource->api);
      if (!empty($api->url)) {
        $url = str_replace($api->url, '', $url);
      }
      $url = $this->url . $url;
    }

    $params += drupal_get_query_array($this->auth);
    $url = url($url, array('query' => $params));

    $request = drupal_http_request($url, array(
      'method' => $method,
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

/**
 * Class default_apihub_handler
 */
class offline_apihub_handler extends apihub_handler {
  /**
   * @param       $method
   * @param       $url
   * @param array $params
   * @param       $resource
   *
   * @return array
   */
  function execute($method, $url, $params = array(), $resource) {
    $data = array();
    $map  = array();

    foreach ($resource->results as $rid => $result) {
      $map[$rid] = & $data;
      if (!empty($result['pid'])) {
        $parent = $result['pid'];
        while ($parent) {
          $map[$rid] = & $map[$parent];

          if (!empty($resource->results[$parent]['pid'])) {
            $parent = $resource->results[$parent]['pid'];
          }
          else {
            unset($parent);
          }
        }
      }

      $key             = empty($result['name']) ? count($map[$rid]) : $result['name'];
      $map[$rid][$key] = $result['type'] == 'group' ? array() : $result['offline'];
      $map[$rid]       = &$map[$rid][$key];
    }

    return $data;
  }
}
