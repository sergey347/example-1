<?php

/**
 * @file
 * Definition of bf_smartystreets_handler_filter_record_type.
 */

/**
 * Filter by all record_type`s.
 *
 * @ingroup views_filter_handlers
 */

class bf_smartystreets_handler_filter_record_type extends views_handler_filter {

  function admin_summary() { }
  function operator_form(&$form, &$form_state) { }
  function can_expose() { return FALSE; }

  function value_form(&$form, &$form_state) {
    parent::value_form();

    $form['value']['possible_types'] = [
      '#type' => 'textfield',
      '#title' => t('Possible record types'),
      '#default_value' => $this->value['possible_types'],
      '#size' => 30,
      '#maxlength' => 60,
    ];

    $form['value']['nullable'] = [
      '#type' => 'checkbox',
      '#title' => t('NULL-able'),
      '#default_value' => $this->value['nullable'],
    ];

    $form['value']['op'] = [
      '#type' => 'select',
      '#title' => t('Operator'),
      '#options' => drupal_map_assoc(['OR', 'AND']),
      '#default_value' => $this->value['op'],
    ];
  }

  function query() {
    $table = $this->ensure_my_table();

    $possible_types = empty($this->value['possible_types'])
      ? " 'F', 'G', 'H', 'P', 'R', 'S' " : $this->value['possible_types'];

    $nullable = empty($this->value['nullable']) ? 'IS NOT NULL' : 'IS NULL';
    $operator = $this->value['op'];

    $statement = "$table.record_type IN ( $possible_types ) $operator $table.record_type $nullable";

    $this->query->add_where_expression($this->options['group'], $statement);
  }
}
