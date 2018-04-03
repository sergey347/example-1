<?php

/**
 * Field handler for addresses.
 */
class bf_smartystreets_handler_response extends views_handler_field {

  /**
   * {@inheritdoc}
   */
  function pre_render(&$values) {
    foreach ($values as $key => &$value) {
      $response =& $value->bf_smartystreets_crm_core_contact_response;
      $decoded = drupal_json_decode($response);

      if ($decoded) {
        $record_type = $decoded[0]['metadata']['record_type'];
        $record_type_title = bf_smartystreets_record_type_explain($record_type);

        $dpv_match_code = $decoded[0]['analysis']['dpv_match_code'];
        $dpv_match_code_title = bf_smartystreets_dpv_match_code_explain($dpv_match_code);

        $dpv_vacant = empty($decoded[0]['analysis']['dpv_vacant']) ? 'U' : $decoded[0]['analysis']['dpv_vacant'];
        $dpv_vacant_title = bf_smartystreets_dpv_vacant_explain($dpv_vacant);

        $variables = [
          'record_type' => [
            'letter' => $record_type,
            'title' => $record_type_title['title'],
          ],
          'dpv_match_code' => [
            'letter' => $dpv_match_code,
            'title' => $dpv_match_code_title['title'],
          ],
          'dpv_vacant' => [
            'letter' => $dpv_vacant,
            'title' => $dpv_vacant_title['title'],
            'extra_class' => ($dpv_vacant == 'U') ? 'undefined' : '',
          ],
        ];

        $response = theme('bf_ss_response', $variables);
      }
      else {
        $response = '<span class="ss-response-wrapper no-response">' . t('Undefined') . '</span>';
      }
    }
  }

  /**
   * {@inheritdoc}
   */
  function render($values) {
    $value = $this->get_value($values);
    return $value;
  }
}
