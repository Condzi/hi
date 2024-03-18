#pragma once

void
calculate_standalone_size(UI_Widget *widget);

void
calculate_upward_depend_size(UI_Widget *widget);

void
calculate_downward_depend_size(UI_Widget *widget);

void
solve_size_violations(UI_Widget *widget);

void
calculate_relative_positions(UI_Widget *widget);

void
calculate_final_positions(UI_Widget *widget);
