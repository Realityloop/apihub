<?php

/**
 * Class apihub_handler
 */
abstract class apihubHandler {
  /**
   * @param        $url
   * @param string $method
   *
   * @return mixed
   */
  abstract public function execute($url, array $parameters = array(), $method = 'GET');

  /**
   * @return mixed
   */
  abstract public function getUrl($url, $path);
}
