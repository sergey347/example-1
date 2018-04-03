
/**
 * JS loaded with "dialer_list_page" view.
 */

(function ($) {

  /**
   * Function select appropriate rows in table according to checked boxes.
   */
  var tableCheck = function ($table, boxClass, boxType, state, disabled) {
    $table.find("tbody tr").each(function(index, elem) {
      var $elem = $(elem),
          litera = $elem.find(boxClass).text().trim(),
          $singleBox = $elem.find(".vbo-select"),
          enabled = disabled ? false : disabled;

      // Disable all boxes in table.
      $singleBox.prop("disabled", disabled);

      // Enable appropriate box and check it.
      switch (boxType) {
        case "record_type":
          if (litera == "H") {
            $singleBox.prop("disabled", enabled)
              .prop("checked", state)
              .click(function() { this.checked = !this.checked; });
          }
          break;
        case "dpv_mc_valid":
          if (litera == "Y" || litera == "D" || litera == "S") {
            $singleBox.prop("disabled", enabled)
              .prop("checked", state)
              .click(function() { this.checked = !this.checked; });
          }
          break;
        case "dpv_mc_not_valid":
          if (litera == "N") {
            $singleBox.prop("disabled", enabled)
              .prop("checked", state)
              .click(function() { this.checked = !this.checked; });
          }
          break;
        case "dpv_vacant":
          if (litera == "Y") {
            $singleBox.prop("disabled", enabled)
              .prop("checked", state)
              .click(function() { this.checked = !this.checked; });
          }
          break;
      }
    });
  };

  /**
   * Main JS behavior for BF smartystreets module.
   */
  Drupal.behaviors.bf_ss_dialer_list_page = {
    attach: function (context, settings) {

      /**
       * Activate Bootstrap tooltip`s.
       */
      if ($.fn.tooltip()) {
        $("[data-toggle=tooltip]", context).tooltip();
      }

      /**
       * Checked boxes disable actions that they do not belong to.
       */
      var action = '[value^="action::bf_smartystreets_remove"]',
          $select = $("#edit-select", context),
          $checkedBoxes = $('.bf-ss-options[type="checkbox"]', $select),
          $table = $("#views-form-dialer-list-page-page table", context),
          $defaultOperation = $(action, $select),
          $operation = $("option", $select).not(action).not('[value="0"]'),
          $selectAllBox = $(".vbo-table-select-all", $table),
          $filteredBoxes = $(".vbo-select", $table).filter(":checked"),
          haveCheckedBoxes = ($filteredBoxes.length > 0);

      // In case checkboxes are already checked.
      if ($checkedBoxes.is(":checked")) {
        $("option", $select)
          .not(action)
          .not('[value="0"]')
          .prop("disabled", "disabled");
      }

      if (haveCheckedBoxes) {
        var alreadyHasNoResponse = $filteredBoxes
          .parents(".views-field-views-bulk-operations")
          .siblings(".views-field-response")
          .find("span")
          .hasClass("no-response");

        if (alreadyHasNoResponse) {
          $defaultOperation.prop("disabled", "disabled");
        }
      }

      $checkedBoxes.change(function() {
        var $this = $(this),
            boxClass = '',
            boxType = $this.val(),
            $siblingsBoxes = $this.parent().siblings().find(".form-checkbox");

        switch (boxType) {
          case "record_type": boxClass = ".record-type"; break;
          case "dpv_mc_valid":
          case "dpv_mc_not_valid": boxClass = ".dpv-match-code"; break;
          case "dpv_vacant": boxClass = ".dpv-vacant"; break;
        }

        if ($this.is(":checked")) {
          // Disable siblings of boxes and inappropriate operations.
          $siblingsBoxes.prop("disabled", "disabled");
          $operation.prop("disabled", "disabled");

          if ($operation.is(":selected")) {
            $defaultOperation.eq(0).prop("selected", "selected");
          }

          tableCheck($table, boxClass, boxType, "checked", "disabled");

          $selectAllBox.prop("disabled", "disabled");
        }
        else {
          // Enable siblings of boxes and inappropriate operations.
          $siblingsBoxes.prop("disabled", false);
          $operation.prop("disabled", false);

          tableCheck($table, boxClass, boxType, false, false);

          $selectAllBox.prop("disabled", false);
        }
      });

      // Disable "Remove from current list" and "Remove to new list"
      // due to checked box with "no response".
      $(".vbo-select", $table).change(function() {
        $this = $(this);
        var hasNoResponse = $this.parents(".views-field-views-bulk-operations")
          .siblings(".views-field-response").find("span").hasClass("no-response");

        if ($this.is(":checked") && hasNoResponse) {
          if ($defaultOperation.is(":selected")) {
            $defaultOperation
              .parents("select")
              .find('[value="0"]')
              .prop("selected", "selected");
          }

          $defaultOperation.prop("disabled", "disabled");
        }
        else if (!$this.is(":checked") && hasNoResponse) {
          var $noResponse = $this
            .parents('tr')
            .siblings()
            .find('.no-response');

          var noResponseChecked = $noResponse.parents(".views-field-response")
            .siblings(".views-field-views-bulk-operations")
            .find(".vbo-select").is(":checked");

          if (!noResponseChecked) {
            $defaultOperation.prop("disabled", false);
          }
        }
      });

      // Disable "Remove from current list" and "Remove to new list"
      // due to checked box with "select all".
      $selectAllBox.change(function() {
        $this = $(this);

        if ($this.is(":checked")) {
          if ($defaultOperation.is(":selected")) {
            $defaultOperation
              .parents("select")
              .find('[value="0"]')
              .prop("selected", "selected");
          }

          $defaultOperation.prop("disabled", "disabled");
        }
        else {
          $defaultOperation.prop("disabled", false);
        }
      });

    }
  };
}(jQuery));
