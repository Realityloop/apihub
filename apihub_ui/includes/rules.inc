<?php
/**
 * @file
 * Rules module integration.
 */

/**
 * Implements hook_apihub_resources_settings() on behalf of rules.module.
 */
function rules_apihub_resources_settings($settings = array()) {
  $options = array();
  $cache   = rules_get_cache();
  foreach ($cache['data_info'] as $data_type => $data_info) {
    $data_info += array('group' => 'Rules');
    $options[$data_info['group']][$data_type] = $data_info['label'];
  }
  asort($options);

  $form['rules']['provides'] = array(
    '#type'  => 'fieldset',
    '#title' => t('Rules - Result'),
  );

  $form['rules']['provides']['name'] = array(
    '#type'          => 'textfield',
    '#title'         => t('Name'),
    '#default_value' => isset($settings['rules']['provides']['name']) ? $settings['rules']['provides']['name'] : '',
    '#required'      => TRUE,
  );

  $form['rules']['provides']['id'] = array(
    '#type'          => 'textfield',
    '#title'         => t('ID'),
    '#default_value' => isset($settings['rules']['provides']['id']) ? $settings['rules']['provides']['id'] : '',
    '#required'      => TRUE,
  );

  $form['rules']['provides']['type'] = array(
    '#type'          => 'select',
    '#title'         => t('Type'),
    '#options'       => $options,
    '#default_value' => isset($settings['rules']['provides']['type']) ? $settings['rules']['provides']['type'] : 'unknown',
    '#required'      => TRUE,
  );

  return $form;
}
