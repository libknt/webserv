#ifndef RULES_HPP
#define RULES_HPP

#ifndef IS_DIGIT
#define IS_DIGIT(chr) ('0' <= chr && chr <= '9')
#endif

#ifndef IS_ALPHA
#define IS_ALPHA(chr) (('a' <= chr && chr <= 'z') || ('A' <= chr && chr <= 'Z'))
#endif

#ifndef IS_VCHAR
#define IS_VCHAR(chr) (33 <= chr && chr <= 127)
#endif

#ifndef IS_TDELIMITAR
#define IS_TDELIMITAR(chr)                                                                         \
	(chr == '(' || chr == ')' || chr == ',' || chr == '/' || chr == ':' || chr == ';' ||           \
		chr == '<' || chr == '>' || chr == '=' || chr == '?' || chr == '@' || chr == '[' ||        \
		chr == '\\' || chr == ']' || chr == '{' || chr == '}')
#endif

#ifndef IS_TCHAR
#define IS_TCHAR(chr) (IS_DIGIT(chr) || IS_ALPHA(chr) || (!IS_TDELIMITAR(chr) && chr != '"'))
#endif

#endif