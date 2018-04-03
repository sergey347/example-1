<?php

/**
 * @file
 * All VBO staff for Backflow Smartystreets module.
 */

/**
 * Implements hook_form_FORM_ID_alter().
 */
function bf_smartystreets_views_bulk_operations_form_alter(&$form, &$form_state, $vbo) {
  $actions = [
    'action::bf_smartystreets_run_smarty_streets_action',
    'action::bf_smartystreets_remove_and_create_action',
    'action::bf_smartystreets_remove_from_current_list_action',
  ];

  $actual_operation = FALSE;

  if (!empty($form_state['operation'])) {
    $actual_operation = in_array($form_state['operation']->operationId, $actions);
  }

  if ($form['#form_id'] == 'views_form_dialer_list_page_page') {
    switch ($form_state['step']) {
      case 'views_form_views_form':
        $options = [
          'record_type' => t('High-rise'),
          'dpv_mc_valid' => t('Valid'),
          'dpv_mc_not_valid' => t('Not Valid'),
          'dpv_vacant' => t('Vacant'),
        ];

        $form['select']['bf_ss_response'] = [
          '#title' => '',
          '#type' => 'checkboxes',
          '#options' => $options,
          '#attributes' => ['class' => ['bf-ss-options']],
        ];

        $form['select']['submit']['#validate'][] = 'bf_smartystreets_vbo_form_validate';

        array_unshift(
          $form['select']['submit']['#submit'],
          'bf_smartystreets_vbo_form_submit'
        );

        break;

      case 'views_bulk_operations_confirm_form':
        if ($actual_operation) {
          array_unshift(
            $form['actions']['submit']['#submit'],
            'bf_smartystreets_vbo_confirm_form_submit'
          );
        }

        break;
    }
  }
}

/**
 * Validate handler.
 */
function bf_smartystreets_vbo_form_validate($form, &$form_state) {
  $op = $form_state['values']['operation'];

  if ($op == 'action::bf_smartystreets_run_smarty_streets_action') {
    $items = $form_state['values']['views_bulk_operations'];
    $before = count($items);
    $after = count(array_filter($items));

    if ($before != $after) {
      $msg = t('Please, choose all rows to perform this operation.');
      form_set_error('views_bulk_operations', $msg);
    }
  }

  if ($op == 'action::bf_smartystreets_remove_and_create_action'
    || $op == 'action::bf_smartystreets_remove_from_current_list_action'
  ) {
    $checked_boxes = array_filter($form_state['values']['bf_ss_response']);
    $count = count($checked_boxes);

    if (empty($count)) {
      $msg = t('Please choose at least one checkbox under select with this operation.');
      form_set_error('bf_ss_response', $msg);
    }
    elseif ($count > 1) {
      $msg = t('Should be checked only one checkbox under select with this operation.');
      form_set_error('bf_ss_response', $msg);
    }

    $form_state['values']['bf_ss_response'] = $checked_boxes;
  }
}

/**
 * Submit handler. View step.
 */
function bf_smartystreets_vbo_form_submit($form, &$form_state) {
  $op = $form_state['values']['operation'];

  switch($op) {
    case 'action::bf_smartystreets_run_smarty_streets_action':
      $form_state['values']['select_all'] = TRUE;

      // Get new view with all results.
      $old_view = $form_state['build_info']['args'][0];
      $view = bf_smartystreets_view_get_all_results($old_view);
      $form_state['build_info']['args'][0] = $view;

      $selection = [];
      foreach ($view->result as $key => $row) {
        $selection[$key] = $row->contact_id;
      }

      // Replace selection array.
      $form_state['values']['views_bulk_operations'] = $selection;
      $form_state['build_info']['args']['origin_selection'] = $selection;

      break;

    case 'action::bf_smartystreets_remove_and_create_action':
    case 'action::bf_smartystreets_remove_from_current_list_action':
      $value = [];
      $value['possible_types'] = NULL;
      $value['nullable'] = NULL;

      $filter_type = '';
      switch (key($form_state['values']['bf_ss_response'])) {
        case 'record_type':
          $value['possible_types'] = "'H'";
          $value['op'] = 'AND';
          $filter_type = 'record_type';
          break;

        case 'dpv_mc_valid':
          $value['possible_types'] = "'Y', 'S', 'D'";
          $value['op'] = 'AND';
          $filter_type = 'dpv_match_code';
          break;

        case 'dpv_mc_not_valid':
          $value['possible_types'] = "'N'";
          $value['op'] = 'AND';
          $filter_type = 'dpv_match_code';
          break;

        case 'dpv_vacant':
          $value['possible_types'] = "'Y'";
          $value['op'] = 'AND';
          $filter_type = 'dpv_vacant';
          break;
      }

      if ($value['possible_types']) {
        $form_state['values']['select_all'] = TRUE;

        // Re-filter view.
        $old_view = $form_state['build_info']['args'][0];
        $view = bf_smartystreets_refilter_view($old_view, $filter_type, $value, 10);
        $form_state['build_info']['args'][0] = $view;

        $selection = [];
        foreach ($view->result as $key => $row) {
          $selection[$key] = $row->contact_id;
        }

        // Replace selection array.
        $form_state['values']['views_bulk_operations'] = $selection;

        // Get original selection array of IDs.
        $origin_view = bf_smartystreets_refilter_view($view, $filter_type, $value, 0);

        $origin_selection = [];
        foreach ($origin_view->result as $key => $row) {
          $origin_selection[$key] = $row->contact_id;
        }
        $form_state['build_info']['args']['origin_selection'] = $origin_selection;

        // Pass Smarty Streets settings.
        $form_state['bf_ss_response'] = $form_state['values']['bf_ss_response'];
      }

      break;
  }
}

/**
 * Submit handler. Confirm step.
 */
function bf_smartystreets_vbo_confirm_form_submit($form, &$form_state) {
  $form_state['selection'] = $form_state['build_info']['args']['origin_selection'];
  $selection = $form_state['selection'];
  $op_id = $form_state['operation']->operationId;

  $create = ($op_id == 'action::bf_smartystreets_remove_and_create_action');
  $remove = ($op_id == 'action::bf_smartystreets_remove_from_current_list_action');

  // Remove from current List and create a new List.
  if ($create && !empty($selection)) {
    $form_state['select_all_pages'] = FALSE;

    // Get appropriate tag info.
    $parent = taxonomy_get_term_by_name('Smarty Streets tags', 'tags');
    $parent = array_values($parent);
    $children = taxonomy_get_children($parent[0]->tid, 1);
    $tags = bf_smartystreets_rearrange_tags($children);

    $response_key = key($form_state['bf_ss_response']);
    $tag_info = $tags[$response_key];

    // Handle old List.
    $old_list_id = $form_state['build_info']['args'][0]->args;
    $old_list = dialer_list_load($old_list_id[0]);
    $old_wrapper = entity_metadata_wrapper('dialer_list', $old_list);
    $old_field_list_contact = $old_wrapper->get('field_list_contact')->value();

    $moved = [];
    foreach ($old_field_list_contact as $key => $entity) {
      if (in_array($entity->contact_id, $selection)) {
        $moved[] = $entity;
        unset($old_field_list_contact[$key]);
      }
    }

    // Set reduced contact_id`s to field_list_contact.
    $old_wrapper->field_list_contact->set($old_field_list_contact);

    // Old filter changes.
    $old_filter = unserialize($old_list->filter);
    $new_filter = $old_filter;

    // Update old List filter.
    $old_filter = bf_smartystreets_update_filter($old_filter, 'none', $tag_info['tid']);

    $old_wrapper->filter->set(serialize($old_filter));
    $old_wrapper->save();

    // Update new List filter.
    $new_filter = bf_smartystreets_update_filter($new_filter, 'or', $tag_info['tid']);

    // Compose a new List.
    $list = [];
    $list['title']     = $old_list->title . $tag_info['suffix'];
    $list['type']      = $old_list->type;
    $list['uid']       = $old_list->uid;
    $list['filter']    = serialize($new_filter);
    $list['status']    = $old_list->status;
    $list['exclude']   = $old_list->exclude;
    $list['timestamp'] = REQUEST_TIME;

    $list_entity = entity_create('dialer_list', $list);
    $wrapper = entity_metadata_wrapper('dialer_list', $list_entity);
    $wrapper->field_list_contact->set($moved);
    $wrapper->save();

    // Add status message.
    $list_info = [
      'id' => $wrapper->getIdentifier(),
      'title' => $list['title'],
    ];
    bf_smartystreets_status_message_arise($list_info);

    // Crutch. Pass "tid" to batch callback.
    $_GET['bf_ss_tid'] = $tag_info['tid'];
  }

  // Remove from current List.
  if ($remove && !empty($selection)) {
    $form_state['select_all_pages'] = FALSE;

    // Get appropriate tag info.
    $parent = taxonomy_get_term_by_name('Smarty Streets tags', 'tags');
    $parent = array_values($parent);
    $children = taxonomy_get_children($parent[0]->tid, 1);
    $tags = bf_smartystreets_rearrange_tags($children);

    $response_key = key($form_state['bf_ss_response']);
    $tag_info = $tags[$response_key];

    // Handle current List.
    $current_list_id = $form_state['build_info']['args'][0]->args;
    $current_list = dialer_list_load($current_list_id[0]);
    $current_wrapper = entity_metadata_wrapper('dialer_list', $current_list);
    $current_field_list_contact = $current_wrapper->get('field_list_contact')->value();

    foreach ($current_field_list_contact as $key => $entity) {
      if (in_array($entity->contact_id, $selection)) {
        unset($current_field_list_contact[$key]);
      }
    }

    // Set reduced contact_id`s to field_list_contact.
    $current_wrapper->field_list_contact->set($current_field_list_contact);

    // Update old List filter.
    $current_filter = unserialize($current_list->filter);
    $current_filter = bf_smartystreets_update_filter($current_filter, 'none', $tag_info['tid']);

    $current_wrapper->filter->set(serialize($current_filter));
    $current_wrapper->save();

    // Crutch. Pass "tid" to batch callback.
    $_GET['bf_ss_tid'] = $tag_info['tid'];
  }
}

/**
 * Helper function. Get view with all results.
 *
 * @see bf_smartystreets_vbo_form_submit().
 */
function bf_smartystreets_view_get_all_results($old_view) {
  $view = views_get_view($old_view->name);
  $view->set_arguments($old_view->args);
  $view->set_display($old_view->current_display);
  $view->set_items_per_page(10);
  $view->is_cacheable = FALSE;
  $view->pre_execute();
  $view->execute();

  return $view;
}

/**
 * Helper function. Re-build "dialer_list_page" view programmatically.
 *
 * @see bf_smartystreets_vbo_form_submit().
 */
function bf_smartystreets_refilter_view($old_view, $filter_type, $filter_value, $items_per_page) {
  $view = views_get_view($old_view->name);
  $view->set_arguments($old_view->args);
  $view->set_display($old_view->current_display);
  $view->set_items_per_page($items_per_page);

  // Not from cache.
  $cache = $view->display_handler->get_plugin('cache');
  $cache->cache_flush();

  // Also do not cache.
  $view->is_cacheable = FALSE;

  // Override filters.
  $filters = $view->display_handler->get_option('filters');
  $filters[$filter_type]['value'] = $filter_value;
  $view->display_handler->override_option('filters', $filters);

  $view->pre_execute();
  $view->execute();

  return $view;
}

/**
 * Helper function.
 */
function bf_smartystreets_update_filter($filter, $op, $tid) {
  if ($field_tags_key = bf_smartystreets_check_field_tags($filter, $op)) {
    // Update existing filter.
    $old_filter[$field_tags_key]['val'][] = $tid;
  }
  else {
    // Insert new filter.
    $filter['count_filter']++;
    $filter[] = [
      'field_type' => 'entity',
      'property'   => 'field_tags',
      'operation'  => $op,
      'val'        => [$tid],
    ];
  }

  return $filter;
}

/**
 * Helper function.
 */
function bf_smartystreets_check_field_tags($filter, $op = 'none') {
  $field_tags_key = NULL;

  foreach ($filter as $key => $value) {
    if (!empty($value['property'])
      && $value['property'] == 'field_tags'
      && $value['operation'] == $op
    ) {
      $field_tags_key = $key;
    }
  }

  return $field_tags_key;
}

/**
 * Helper function.
 */
function bf_smartystreets_rearrange_tags($tags) {
  $rearranged = [];

  foreach ($tags as $tag) {
    switch ($tag->name) {
      case BF_SS_HIGH_RISE:
        $rearranged['record_type'] = [
          'tid' => $tag->tid,
          'suffix' => ' (High-rise)',
        ];
        break;

      case BF_SS_VALID:
        $rearranged['dpv_mc_valid'] = [
          'tid' => $tag->tid,
          'suffix' => ' (Valid)',
        ];
        break;

      case BF_SS_NOT_VALID:
        $rearranged['dpv_mc_not_valid'] = [
          'tid' => $tag->tid,
          'suffix' => ' (Not-valid)',
        ];
        break;

      case BF_SS_VACANT:
        $rearranged['dpv_vacant'] = [
          'tid' => $tag->tid,
          'suffix' => ' (Vacant)',
        ];
        break;
    }
  }

  return empty($rearranged) ? NULL : $rearranged;
}

/**
 * Add status message about newly created List after performing VBO operations.
 */
function bf_smartystreets_status_message_arise($list_info) {
  $pure_link = 'dialer-list/' . $list_info['id'];
  $link = l($list_info['title'], $pure_link);
  $msg = t("List !link was created successfully!", ['!link' => $link]);

  $_SESSION['messages']['status'][] = $msg;
}
