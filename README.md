#### Example of Smarty Streets API Integration

This module provides us 3 actions: 

1. bf_smartystreets_run_smarty_streets_action
2. bf_smartystreets_remove_and_create_action
3. bf_smartystreets_remove_from_current_list_action

```
Actions are using via VBO views field.

First of all you run [1] action and see the result in "Smarty Streets response" column 
in current "Dialer List" view.

Now, regarding on result you got, you can perform [2] or [3] action. 
[2] action will move contacts from current List to newly created.
[3] action will permanently remove contacts from current List.

Smarty Streets response is stored in table "bf_smartystreets" according to each contact item. 
```