<?php
/**
 * @file
 * API Hub module integration.
 */

/**
 * Implements hook_apihub_handler_info().
 */
function apihub_apihub_handler_info() {
  return array('apihubHandlerDrupal' => t('Drupal'));
}

/**
 * Implement hook_apihub_pre_execute().
 */
function apihub_apihub_pre_execute($method, $apihub) {
  // @TODO - Also check if the whole API is offline.
  if (isset($method->settings['apihub']['offline']) && $method->settings['apihub']['offline']) {
    drupal_set_message(t('Method "@method" is in offline mode.', array(
      '@method' => $method->admin_title,
    )), 'status', FALSE);

    $apihub->setHandler(new offline_apihub_handler());
  }
}

/**
 * Class default_apihub_handler
 */
//class offline_apihub_handler extends apihubHandler {
//  /**
//   * @param       $method
//   * @param       $url
//   * @param array $params
//   * @param       $method
//   *
//   * @return array
//   */
//  function execute($method, $url, $params = array(), $method) {
//    $data = array();
//    $map  = array();
//
//    foreach ($method->results as $rid => $result) {
//      $map[$rid] = &$data;
//      if (!empty($result['pid'])) {
//        $parent = $result['pid'];
//        while ($parent) {
//          $map[$rid] = &$map[$parent];
//
//          if (!empty($method->results[$parent]['pid'])) {
//            $parent = $method->results[$parent]['pid'];
//          }
//          else {
//            unset($parent);
//          }
//        }
//      }
//
//      $key             = empty($result['name']) ? count($map[$rid]) : $result['name'];
//      $map[$rid][$key] = $result['type'] == 'group' ? array() : $result['offline'];
//      $map[$rid]       = &$map[$rid][$key];
//    }
//
//    return $data;
//  }
//}
