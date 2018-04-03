<?php

/**
 * @file
 * Admin forms for bf_smartystreets module.
 */

/**
 * Admin settings form.
 */
function bf_smartystreets_admin_settings($form, &$form_state) {
  $form['bf_ss_auth_id'] = [
    '#type' => 'textfield',
    '#title' => t('Auth ID'),
    '#default_value' => variable_get('bf_ss_auth_id'),
    '#description' => t('Smartystreets Auth ID.'),
    '#required' => TRUE,
  ];

  $form['bf_ss_auth_token'] = [
    '#type' => 'textfield',
    '#title' => t('Auth Token'),
    '#default_value' => variable_get('bf_ss_auth_token'),
    '#description' => t('Smartystreets Auth Token.'),
    '#required' => TRUE,
  ];

  return system_settings_form($form);
}
