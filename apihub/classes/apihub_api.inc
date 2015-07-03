<?php

/**
 * Class apihubApi
 */
class apihubApi {

  /**
   * @param $key
   * @param $value
   */
  public function __set($key, $value) {
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
  public function __toString() {
    return $this->name;
  }
}
