internal Launch_Opt *gOptions;

global void
parse_launch_opts(Str8 *opts, u64 count) {
  // Launch options have following syntax:
  //
  //  --MyOpt value --OtherOpt --AnotherOpt value
  //
  // Currently, we don't bother with string literals etc.
  //

  for (u64 i = 0; i < count; i++) {
    Str8 name = opts[i];
    if (str8_has_prefix(name, str8_lit("--"))) {
      name = str8_range(name.v + 2, name.v + name.sz);
    }

    // Check the next available token. If it has a prefix of '--', assume that this
    // argument is a flag option.
    //
    Str8 val = {};
    if (i + 1 < count) {
      val = opts[i + 1];
      if (str8_has_prefix(val, str8_lit("--"))) {
        val = {};
      } else {
        i++;
      }
    }

    push_launch_opt(name, val);
  }
}

must_use global bool
has_launch_opt(Str8 name) {
  Launch_Opt *it = gOptions;
  while (it) {
    if (str8_compare(it->name, name)) {
      return true;
    }
    it = it->next;
  }
  return false;
}

must_use global Str8
get_launch_opt(Str8 name) {
  Launch_Opt *it = gOptions;
  while (it) {
    if (str8_compare(it->name, name)) {
      return it->value;
    }
    it = it->next;
  }

  return str8_lit("none");
}

global void
push_launch_opt(Str8 name, Str8 value) {
  Launch_Opt *opt = arena_alloc<Launch_Opt>(tcx.misc_arena);
  opt->name       = name;
  opt->value      = value;

  SLL_insert(gOptions, opt);

  os_debug_message(name);
  os_debug_message(str8_lit(" = "));
  os_debug_message(value);
  os_debug_message(str8_lit("\n"));
}
