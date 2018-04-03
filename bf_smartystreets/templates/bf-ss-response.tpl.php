<?php
/**
 * Template file: bf-ss-response.tpl.php
 */
?>

<span class="ss-response-wrapper">
  <span class="ss-response record-type" title="<?php print $record_type['title']; ?>" data-toggle="tooltip">
    <?php print $record_type['letter']; ?>
  </span>
  <span class="ss-response dpv-match-code" title="<?php print $dpv_match_code['title']; ?>" data-toggle="tooltip">
    <?php print $dpv_match_code['letter']; ?>
  </span>
  <span class="ss-response dpv-vacant last <?php print $dpv_vacant['extra_class']; ?>" title="<?php print $dpv_vacant['title']; ?>" data-toggle="tooltip">
    <?php print $dpv_vacant['letter']; ?>
  </span>
</span>
