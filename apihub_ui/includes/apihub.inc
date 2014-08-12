<?php
/**
 * @file
 * API Hub module integration.
 */

/**
 * Implements hook_apihub_resources_settings() on behalf of views.module.
 */
function apihub_apihub_resources_settings($settings = array(), $item) {
  $form = array();

  // Views integration only works with GET requests.
  if (isset($item->method) && $item->method == 'GET') {
    $form['offline'] = array(
      '#type'          => 'checkbox',
      '#title'         => t('Offline mode'),
      '#description'   => t('Puts the current resource into offline mode.'),
      '#default_value' => isset($settings['apihub']['offline']) ? $settings['apihub']['offline'] : 0,
    );
  }

  return $form;
}
