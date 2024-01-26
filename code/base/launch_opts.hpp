#pragma once

struct Launch_Opt {
  Launch_Opt *next;

  Str8 name;
  Str8 value;
};

global void
parse_launch_opts(Str8* opts, u64 count);

must_use global bool
has_launch_opt(Str8 name);

must_use global Str8
get_launch_opt(Str8 name);

global void
push_launch_opt(Str8 name, Str8 value);
