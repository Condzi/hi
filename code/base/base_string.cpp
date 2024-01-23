must_use Str8
str8(u8 *v, u64 sz) {
  return {.v = v, .sz = sz};
}

must_use Str8
str8_range(u8 *beg, u8 *end) {
  return {.v = beg, .sz = PtrToU64(end) - PtrToU64(begin)};
}

must_use Str8
str8_cstr(char const *cstr) {
  u64 const len = strlen(cstr);
  return {.v = (u8 *)cstr, .sz = len};
}

must_use Str16
str16(u16 *v, u64 sz) {
  return {.v = v, .sz = sz};
}

must_use Str16
str16_range(u16 *beg, u16 *end) {
  return {.v = beg, .sz = PtrToU64(end) - PtrToU64(begin)};
}
