<?php

/**
 * Class apihub_ui
 */
class apihub_ui extends ctools_export_ui {

  /**
   * Builds the operation links for a specific exportable item.
   */
  function build_operations($item) {
    $allowed_operations = parent::build_operations($item);

    // Ensure 'Edit' is the first operation.
    $edit = array($allowed_operations['edit']);
    unset($allowed_operations['edit']);
    $allowed_operations = array_merge($edit, $allowed_operations);

    return $allowed_operations;
  }

  /**
   * Provide the actual editing form.
   */
  function edit_form(&$form, &$form_state) {
    parent::edit_form($form, $form_state);

    $form['info']['admin_title']['#title']       = t('Title');
    $form['info']['admin_description']['#title'] = t('Description');
  }

}

/**
 * Class apihub_apis_ui
 */
class apihub_apis_ui extends apihub_ui {

  /**
   * Provide the actual editing form.
   */
  function edit_form(&$form, &$form_state) {
    parent::edit_form($form, $form_state);

    $export_key = $this->plugin['export']['key'];
    $item       = $form_state['item'];
    $schema     = ctools_export_get_schema($this->plugin['schema']);

    $form['info']['url'] = array(
      '#title'         => t('API URL'),
      '#type'          => 'textfield',
      '#default_value' => $item->url,
    );
  }
}
