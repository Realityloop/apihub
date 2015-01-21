<?php
/**
 * @file
 * Views module integration.
 */

/**
 * Implements hook_apihub_resources_settings() on behalf of views.module.
 */
function views_apihub_resources_settings($settings = array(), $item) {
  $form = array();

  // Views integration only works with GET requests.
  if (isset($item->method) && $item->method == 'GET') {
    $form['enabled'] = array(
      '#type'          => 'checkbox',
      '#title'         => t('Views integration'),
      '#description'   => t('Adds ability to pull results straight into Views.'),
      '#default_value' => isset($settings['views']['enabled']) ? $settings['views']['enabled'] : 0,
    );
  }

  return $form;
}
