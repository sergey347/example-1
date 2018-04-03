<?php

/**
 * @file
 * Provide views data and handlers for bf_smartystreets.module.
 */

/**
 * Implements hook_views_data().
 */
function bf_smartystreets_views_data() {
  $data = [];

  $data['bf_smartystreets']['table']['group']  = t('BF Smarty Streets');
  $data['bf_smartystreets']['table']['base'] = [
    'title' => t('Smarty Streets - Responses'),
    'help' => t('Contains records we want exposed to Views.'),
  ];

  $data['bf_smartystreets']['entity_id'] = [
    'title' => t('Contact ID'),
    'field' => [
      'handler' => 'views_handler_field',
      'help' => t('Contact ID related to Smarty Streets response.'),
    ],
  ];

  $data['bf_smartystreets']['response'] = [
    'title' => t('Response'),
    'field' => [
      'handler' => 'bf_smartystreets_handler_response',
      'help' => t('Response from Smarty Streets.'),
    ],
  ];

  $data['bf_smartystreets']['full_address'] = [
    'title' => t('Full Address'),
    'field' => [
      'handler' => 'views_handler_field',
      'help' => t('Full address.'),
    ],
  ];

  $data['bf_smartystreets']['full_address_verified'] = [
    'title' => t('Full Address verified'),
    'field' => [
      'handler' => 'views_handler_field',
      'help' => t('Full address verified.'),
    ],
  ];

  $data['bf_smartystreets']['record_type'] = [
    'title' => t('Record type'),
    'field' => [
      'handler' => 'views_handler_field',
      'help' => t('Record type from response.'),
    ],
    'filter' => [
      'handler' => 'bf_smartystreets_handler_filter_record_type',
      'help' => t('Filter by record_type from response.'),
    ],
  ];

  $data['bf_smartystreets']['dpv_match_code'] = [
    'title' => t('DPV match code'),
    'field' => [
      'handler' => 'views_handler_field',
      'help' => t('DPV match code from response.'),
    ],
    'filter' => [
      'handler' => 'bf_smartystreets_handler_filter_dpv_match_code',
      'help' => t('Filter by dpv_match_code from response.'),
    ],
  ];

  $data['bf_smartystreets']['dpv_vacant'] = [
    'title' => t('DPV vacant'),
    'field' => [
      'handler' => 'views_handler_field',
      'help' => t('Record type from response.'),
    ],
    'filter' => [
      'handler' => 'bf_smartystreets_handler_filter_dpv_vacant',
      'help' => t('Filter by dpv_vacant from response.'),
    ],
  ];

  return $data;
}

/**
 * Implements hook_views_data_alter().
 */
function bf_smartystreets_views_data_alter(&$data) {
  $data['crm_core_contact']['entity_id'] = [
    'title' => t('Smarty Streets relation'),
    'help' => t('Relation data.'),
    'type' => 'string',

    'relationship' => [
      'title' => t('Smarty Streets'),
      'help' => t('Relate on Smarty Streets responses table.'),

      'handler' => 'views_handler_relationship',
      'base' => 'bf_smartystreets',
      'base field' => 'entity_id',
      'field' => 'contact_id',
      'label' => t('Smarty Streets'),
    ],
  ];
}
