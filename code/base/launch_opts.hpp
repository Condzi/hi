#pragma once

struct Launch_Opt {
  Launch_Opt *next;

  Str8 name;
  Str8 value;
};

void
parse_launch_opts(Str8 *opts, u64 count);

must_use bool
has_launch_opt(Str8 name);

must_use Str8
get_launch_opt(Str8 name);

void
push_launch_opt(Str8 name, Str8 value);
