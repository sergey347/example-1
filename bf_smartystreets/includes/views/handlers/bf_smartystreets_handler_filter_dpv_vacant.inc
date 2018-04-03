<?php

/**
 * @file
 * Definition of bf_smartystreets_handler_filter_dpv_vacant.
 */

/**
 * Filter by all dpv_vacant`s.
 *
 * @ingroup views_filter_handlers
 */

class bf_smartystreets_handler_filter_dpv_vacant extends views_handler_filter {

  /**
   * {@inheritdoc}
   */
  function admin_summary() { }
  function operator_form(&$form, &$form_state) { }
  function can_expose() { return FALSE; }

  /**
   * {@inheritdoc}
   */
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

  /**
   * {@inheritdoc}
   */
  function query() {
    $table = $this->ensure_my_table();

    $possible_types = empty($this->value['possible_types'])
      ? " 'Y', 'N' " : $this->value['possible_types'];

    $nullable = empty($this->value['nullable']) ? 'IS NOT NULL' : 'IS NULL';
    $operator = $this->value['op'];

    $statement = "$table.dpv_vacant IN ( $possible_types ) $operator $table.dpv_vacant $nullable";

    $this->query->add_where_expression($this->options['group'], $statement);
  }
}
