<?php
/**
 * @file
 * Features module integration.
 */

/**
 * @return array
 */
function apihub_apihub_methods_list() {
  $list = array();

  $apis       = ctools_export_crud_load_all('apihub_apis');
  $items      = ctools_export_crud_load_all('apihub_methods');
  $schema     = ctools_export_get_schema('apihub_methods');
  $export_key = $schema['export']['key'];
  foreach ($items as $item) {
    $list[$item->$export_key] = check_plain(t('@api: @admin_title (@http_method @path)', array(
      '@api'         => $item->api->admin_title,
      '@admin_title' => $item->admin_title,
      '@http_method' => $item->http_method,
      '@path'        => $item->path,
    )));
  }

  return $list;
}

/**
 * Implements hook_features_pipe_COMPONENT_alter().
 */
function apihub_ui_features_pipe_apihub_apis_alter(&$pipe, $data, $export) {
  foreach ($data as $api) {
    $methods                = ctools_export_load_object('apihub_methods', 'conditions', array('api' => $api));
    $pipe['apihub_methods'] = array_keys($methods);
  }
}
