<?php
/**
 * @file
 * Field module integration.
 */

/**
 * Implements hook_field_info().
 */
function apihub_field_info() {
  return array(
    'apihub_handler' => array(
      'label'             => t('API Hub handler'),
      'description'       => t(''),
      'default_widget'    => 'apihub_handler',
      'default_formatter' => 'apihub_handler',
    ),
  );
}

/**
 * Implements hook_field_is_empty().
 */
function apihub_field_is_empty($item, $field) {
  return FALSE;
}

/**
 * Implements hook_field_formatter_info().
 */
function apihub_field_formatter_info() {
  return array(
    'apihub_handler' => array(
      'label'       => t('API Hub handler'),
      'field types' => array('apihub_handler'),
    ),
  );
}

/**
 * Implements hook_field_formatter_view().
 */
function apihub_field_formatter_view($entity_type, $entity, $field, $instance, $langcode, $items, $display) {
  $elements = array();

  // @TODO - Basic formatter needs work.
  switch ($display['type']) {
    case 'apihub_handler':
      foreach ($items as $delta => $item) {
        $elements[$delta]['class'] = array(
          '#type'   => 'item',
          '#title'  => 'handler',
          '#markup' => $item['class'],
        );
        foreach ($item['settings'] as $setting => $value) {
          $elements[$delta]['settings'][$setting] = array(
            '#type'   => 'item',
            '#title'  => $setting,
            '#markup' => $value,
          );
        }
      }
      break;
  }

  return $elements;
}

/**
 * Implements hook_field_widget_info().
 */
function apihub_field_widget_info() {
  return array(
    'apihub_handler' => array(
      'label'       => t('API Hub handler'),
      'field types' => array('apihub_handler'),
    ),
  );
}

/**
 * Implements hook_field_widget_form().
 */
function apihub_field_widget_form(&$form, &$form_state, $field, $instance, $langcode, $items, $delta, $element) {
  switch ($instance['widget']['type']) {
    case 'apihub_handler':
      $element += apihub_handlers_form($items[0], FALSE);
      break;
  }

  return $element;
}

/**
 * Implements hook_field_load().
 */
function apihub_field_load($entity_type, $entities, $field, $instances, $langcode, &$items, $age) {
  foreach ($entities as $id => $entity) {
    foreach ($items[$id] as $delta => $item) {
      $items[$id][$delta]['settings'] = !empty($items[$id][$delta]['settings']) ? unserialize($items[$id][$delta]['settings']) : array();
    }
  }
}

/**
 * Implements hook_field_insert().
 */
function apihub_field_insert($entity_type, $entity, $field, $instance, $langcode, &$items) {
  foreach ($items as $delta => $item) {
    $items[$delta]['settings'] = serialize($items[$delta]['settings']);
  }
}

/**
 * Implements hook_field_update().
 */
function apihub_field_update($entity_type, $entity, $field, $instance, $langcode, &$items) {
  apihub_field_insert($entity_type, $entity, $field, $instance, $langcode, $items);
}
