
void
calculate_standalone_size(UI_Widget *widget) {
  UI_Size const &sz_x    = widget->semantic_size[Axis2_X];
  UI_Size const &sz_y    = widget->semantic_size[Axis2_Y];
  f32           &sz_px_x = widget->sz_px.x;
  f32           &sz_px_y = widget->sz_px.y;

  if (sz_x.kind == UI_SizeKind_Pixels) {
    sz_px_x = sz_x.value;
  } else if (sz_x.kind == UI_SizeKind_TextContent) {
    f32 const w = ui_size_text(widget->string).x;
    sz_px_x     = w;
  }

  if (sz_y.kind == UI_SizeKind_Pixels) {
    sz_px_y = sz_y.value;
  } else if (sz_y.kind == UI_SizeKind_TextContent) {
    f32 const h = ui_size_text(widget->string).y;
    sz_px_y     = h;
  }
}

void
calculate_upward_depend_size(UI_Widget *widget) {
  UI_Size const &sz_x    = widget->semantic_size[Axis2_X];
  UI_Size const &sz_y    = widget->semantic_size[Axis2_Y];
  f32           &sz_px_x = widget->sz_px.x;
  f32           &sz_px_y = widget->sz_px.y;

  if (sz_x.kind == UI_SizeKind_PercentOfParent) {
    f32 const parent_w = widget->parent->sz_px.x;
    sz_px_x            = sz_x.value * parent_w;
  }

  if (sz_y.kind == UI_SizeKind_PercentOfParent) {
    f32 const parent_h = widget->parent->sz_px.y;
    sz_px_y            = sz_y.value * parent_h;
  }
}

void
calculate_downward_depend_size(UI_Widget *widget) {
  UI_Size const &sz_x    = widget->semantic_size[Axis2_X];
  UI_Size const &sz_y    = widget->semantic_size[Axis2_Y];
  f32           &sz_px_x = widget->sz_px.x;
  f32           &sz_px_y = widget->sz_px.y;

  if (sz_x.kind == UI_SizeKind_ChildrenSum) {
    f32        sum = 0;
    UI_Widget *it  = widget->first_child;
    while (it) {
      if (it->flags & UI_WidgetFlag_HorizontalLayout) {
        sum += it->sz_px.x;
      } else if (it->sz_px.x > sum) {
        sum = it->sz_px.x;
      }
      it = it->next;
    }
    sz_px_x = sum;
  }

  if (sz_y.kind == UI_SizeKind_ChildrenSum) {
    f32        sum = 0;
    UI_Widget *it  = widget->first_child;
    while (it) {
      if (it->flags & UI_WidgetFlag_HorizontalLayout) {
        if (it->sz_px.y > sum) {
          sum = it->sz_px.y;
        }
      } else {
        sum += it->sz_px.y;
      }
      it = it->next;
    }
    sz_px_y = sum;
  }
}

must_use internal f32
anim_curve(f32 t) {
  f32 const ttt = (1 - t) * (1 - t) * (1 - t);
  return 1 - ttt;
}

void
solve_size_violations(UI_Widget *widget) {
  widget->sz_final = widget->sz_px;
  if (widget->flags & UI_WidgetFlag_AnimateHorizontal) {
    widget->sz_final.x *= anim_curve(widget->hot_anim);
  }
  if (widget->flags & UI_WidgetFlag_AnimateVertical) {
    widget->sz_final.y *= anim_curve(widget->hot_anim);
  }

  if (!widget->parent) {
    // It's the root node.
    return;
  }
  fvec2 const parent_sz = widget->parent->sz_final;

  // How big children want to be.
  fvec2      wanted = {};
  UI_Widget *it     = widget->first_child;
  while (it) {
    if (it->flags & UI_WidgetFlag_HorizontalLayout) {
      wanted.x += it->sz_px.x;
    } else {
      wanted.y += it->sz_px.y;
    }
    if (it->sz_px.x > wanted.x) {
      wanted.x = it->sz_px.x;
    }
    it = it->next;
  }

  if (wanted.x > parent_sz.x) {
    f32 const violation = wanted.x - parent_sz.x;
    it                  = widget->first_child;
    while (it) {
      f32 const multiplier = 1 - it->semantic_size[Axis2_X].strictness;
      f32 const adjustment = violation * multiplier;
      it->sz_final.x -= adjustment;
      it = it->next;
    }
  }
  if (wanted.y > parent_sz.y) {
    f32 const violation = wanted.y - parent_sz.y;
    it                  = widget->first_child;
    while (it) {
      f32 const multiplier = 1 - it->semantic_size[Axis2_Y].strictness;
      f32 const adjustment = violation * multiplier;
      it->sz_final.y -= adjustment;
      it = it->next;
    }
  }
}

void
calculate_relative_positions(UI_Widget *widget) {
  fvec2      pen     = {};
  UI_Widget *it      = widget->first_child;
  fvec2      prev_sz = {};
  while (it) {
    if (it->flags & UI_WidgetFlag_HorizontalLayout) {
      pen.x += prev_sz.x;
      prev_sz.y = Max(it->sz_final.y, prev_sz.y);
    } else {
      pen.x = 0;
      pen.y += prev_sz.y;
      prev_sz = it->sz_final;
    }
    it->pos_rel = pen;
    it          = it->next;
  }
}

void
calculate_final_positions(UI_Widget *widget) {
  if (!widget->parent) {
    // Root
    widget->pos_final = {}; // Top - left
    return;
  }

  // @Note: We need to remember that we're using Cartesian coordinate system (X-> Y^) in renderer,
  // but X-> Y\/ in UI.
  fvec2 const parent_pos = widget->parent->pos_final;
  fvec2 const rel_pos    = widget->pos_rel;
  fvec2       final_pos  = {};
  final_pos.x            = parent_pos.x + rel_pos.x;
  final_pos.y            = parent_pos.y - rel_pos.y;
  widget->pos_final      = final_pos;
}
