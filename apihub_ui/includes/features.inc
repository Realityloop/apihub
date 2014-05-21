<?php
/**
 * @file
 * Features module integration.
 */

/**
 * @return array
 */
function apihub_apihub_resources_list() {
  $list = array();

  $apis       = ctools_export_crud_load_all('apihub_apis');
  $items      = ctools_export_crud_load_all('apihub_resources');
  $schema     = ctools_export_get_schema('apihub_resources');
  $export_key = $schema['export']['key'];
  foreach ($items as $item) {
    $list[$item->$export_key] = check_plain(t('@api: @admin_title (@method @path)', array(
      '@api'         => $apis[$item->api]->admin_title,
      '@admin_title' => $item->admin_title,
      '@method'      => $item->method,
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
    $resources = ctools_export_load_object('apihub_resources', 'conditions', array('api' => $api));
    $pipe['apihub_resources'] = array_keys($resources);
  }
}
