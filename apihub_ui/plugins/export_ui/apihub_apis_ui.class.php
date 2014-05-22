<?php

/**
 * Class apihub_apis_ui
 */
class apihub_apis_ui extends ctools_export_ui {

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
