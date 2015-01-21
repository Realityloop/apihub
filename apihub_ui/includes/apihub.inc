<?php
/**
 * @file
 * API Hub module integration.
 */

/**
 * Implements hook_apihub_resources_settings() on behalf of apihub.module.
 */
function apihub_apihub_resources_settings($settings = array(), $item) {
  $form = array();

  $form['root'] = array(
    '#type'          => 'textfield',
    '#title'         => t('Root element'),
    '#description'   => t('The root element to be used for iterators.'),
    '#default_value' => isset($settings['apihub']['root']) ? $settings['apihub']['root'] : '',
  );

  $form['pager'] = array(
    '#type'  => 'fieldset',
    '#title' => t('Pager'),
  );

  $form['pager']['total'] = array(
    '#type'          => 'textfield',
    '#title'         => t('Total count element'),
    '#description'   => t('An element containing the total count of items.'),
    '#default_value' => isset($settings['apihub']['pager']['total']) ? $settings['apihub']['pager']['total'] : '',
  );

  $form['pager']['offset'] = array(
    '#type'          => 'textfield',
    '#title'         => t('Offset element'),
    '#description'   => t('An element containing the current item offset.'),
    '#default_value' => isset($settings['apihub']['pager']['offset']) ? $settings['apihub']['pager']['offset'] : '',
  );

  $form['offline'] = array(
    '#type'          => 'checkbox',
    '#title'         => t('Offline mode'),
    '#description'   => t('Puts the current resource into offline mode.'),
    '#default_value' => isset($settings['apihub']['offline']) ? $settings['apihub']['offline'] : 0,
  );

  return $form;
}
