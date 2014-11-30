<?php

/**
 * Class apihub_request
 */
class apihub_request {
  protected $api = NULL;
  protected $handler = NULL;

  /**
   * @param        $api
   * @param string $handler
   * @param array  $args
   */
  function __construct($api, $handler = 'default', $args = array()) {
    ctools_include('export');
    $this->api = ctools_export_crud_load('apihub_apis', $api);

    $handler = "{$handler}_apihub_handler";
    if (class_exists($handler)) {
      $this->handler = new $handler($args);
    }
  }

  /**
   * @param       $method
   * @param       $name
   * @param array $params
   *
   * @return mixed
   */
  function execute($method, $name, $params = array()) {
    $method = strtoupper($method);
    $id     = md5("{$this->api->name}::{$method}::{$name}");

    ctools_include('export');
    $resource = ctools_export_crud_load('apihub_resources', $id);
    $url      = $this->api->url . $resource->path;

    // Let third party module respond before resource is executed.
    module_invoke_all('apihub_pre_execute', $resource, $this);

    // Cleanup parameters.
    // @TODO - Ensure required parameters are provided and not empty.
    $params = is_null($params) ? array() : $params;
    foreach ($params as $key => $value) {
      if ((empty($params[$key]) && !is_numeric($params[$key])) || is_null($params[$key])) {
        unset($params[$key]);
      }
    }

    // Substitute parameters in URL.
    foreach ($params as $key => $value) {
      foreach (array(":{$key}", "[{$key}]") as $needle) {
        if (strstr($url, $needle)) {
          $url = str_replace($needle, $value, $url);
          unset($params[$key]);
          break;
        }
      }
    }

    return $this->handler->execute($method, $url, $params, $resource);
  }

  function setHandler($handler) {
    $this->handler = $handler;
  }
}

/**
 * Class apihub_handler
 */
abstract class apihub_handler {
  abstract protected function execute($method, $url, $params = array(), $resource);
}
