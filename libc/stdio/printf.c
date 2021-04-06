#include <stdio.h>
#include <string.h>
#include <va_list.h>

static void print_dec(unsigned long long value, unsigned int width, char * buf, int * ptr, int fill_zero, int align_right, int precision) {
	unsigned long long n_width = 1;
	unsigned long long i = 9;
	if (precision == -1) precision = 1;

	if (value == 0) {
		n_width = 0;
	} else {
		unsigned long long val = value;
		while (val >= 10UL) {
			val /= 10UL;
			n_width++;
		}
	}

	if (n_width < (unsigned long long)precision) n_width = precision;

	int printed = 0;
	if (align_right) {
		while (n_width + printed < width) {
			buf[*ptr] = fill_zero ? '0' : ' ';
			*ptr += 1;
			printed += 1;
		}

		i = n_width;
		while (i > 0) {
			unsigned long long n = value / 10;
			long long r = value % 10;
			buf[*ptr + i - 1] = r + '0';
			i--;
			value = n;
		}
		*ptr += n_width;
	} else {
		i = n_width;
		while (i > 0) {
			unsigned long long n = value / 10;
			long long r = value % 10;
			buf[*ptr + i - 1] = r + '0';
			i--;
			value = n;
			printed++;
		}
		*ptr += n_width;
		while (printed < (long long)width) {
			buf[*ptr] = fill_zero ? '0' : ' ';
			*ptr += 1;
			printed += 1;
		}
	}
}

/*
 * Hexadecimal to string
 */
static void print_hex(unsigned long long value, unsigned int width, char * buf, int * ptr) {
	int i = width;

	if (i == 0) i = 8;

	unsigned long long n_width = 1;
	unsigned long long j = 0x0F;
	while (value > j && j < UINT64_MAX) {
		n_width += 1;
		j *= 0x10;
		j += 0x0F;
	}

	while (i > (long long)n_width) {
		buf[*ptr] = '0';
		*ptr += 1;
		i--;
	}

	i = (long long)n_width;
	while (i-- > 0) {
		buf[*ptr] = "0123456789abcdef"[(value>>(i*4))&0xF];
		*ptr += + 1;
	}
}

/*
 * vasprintf()
 */
int xvasprintf(char * buf, const char * fmt, va_list args) {
	int i = 0;
	char * s;
	char * b = buf;
	int precision = -1;
	for (const char *f = fmt; *f; f++) {
		if (*f != '%') {
			*b++ = *f;
			continue;
		}
		++f;
		unsigned int arg_width = 0;
		int align = 1; /* right */
		int fill_zero = 0;
		int big = 0;
		int alt = 0;
		int always_sign = 0;
		while (1) {
			if (*f == '-') {
				align = 0;
				++f;
			} else if (*f == '#') {
				alt = 1;
				++f;
			} else if (*f == '*') {
				arg_width = (char)va_arg(args, int);
				++f;
			} else if (*f == '0') {
				fill_zero = 1;
				++f;
			} else if (*f == '+') {
				always_sign = 1;
				++f;
			} else {
				break;
			}
		}
		while (*f >= '0' && *f <= '9') {
			arg_width *= 10;
			arg_width += *f - '0';
			++f;
		}
		if (*f == '.') {
			++f;
			precision = 0;
			if (*f == '*') {
				precision = (int)va_arg(args, int);
				++f;
			} else  {
				while (*f >= '0' && *f <= '9') {
					precision *= 10;
					precision += *f - '0';
					++f;
				}
			}
		}
		if (*f == 'l') {
			big = 1;
			++f;
			if (*f == 'l') {
				big = 2;
				++f;
			}
		}
		if (*f == 'z') {
			big = 1;
			++f;
		}
		/* fmt[i] == '%' */
		switch (*f) {
			case 's': /* String pointer -> String */
				{
					size_t count = 0;
					if (big) {
						return -1;
					} else {
						s = (char *)va_arg(args, char *);
						if (s == NULL) {
							s = "(null)";
						}
						if (precision >= 0) {
							while (*s && precision > 0) {
								*b++ = *s++;
								count++;
								precision--;
								if (arg_width && count == arg_width) break;
							}
						} else {
							while (*s) {
								*b++ = *s++;
								count++;
								if (arg_width && count == arg_width) break;
							}
						}
					}
					while (count < arg_width) {
						*b++ = ' ';
						count++;
					}
				}
				break;
			case 'c': /* Single character */
				*b++ = (char)va_arg(args, int);
				break;
			case 'p':
				if (!arg_width) {
					arg_width = 8;
					alt = 1;
					if (sizeof(void*) == sizeof(long long)) big = 2;
				}
			case 'x': /* Hexadecimal number */
				{
					if (alt) {
						*b++ = '0';
						*b++ = 'x';
					}
					i = b - buf;
					unsigned long long val;
					if (big == 2) {
						val = (unsigned long long)va_arg(args, unsigned long long);
					} else if (big == 1) {
						val = (unsigned long)va_arg(args, unsigned long);
					} else {
						val = (unsigned int)va_arg(args, unsigned int);
					}
					print_hex(val, arg_width, buf, &i);
					b = buf + i;
				}
				break;
			case 'i':
			case 'd': /* Decimal number */
				{
					long long val;
					if (big == 2) {
						val = (long long)va_arg(args, long long);
					} else if (big == 1) {
						val = (long)va_arg(args, long);
					} else {
						val = (int)va_arg(args, int);
					}
					if (val < 0) {
						*b++ = '-';
						val = -val;
					} else if (always_sign) {
						*b++ = '+';
					}
					i = b - buf;
					print_dec(val, arg_width, buf, &i, fill_zero, align, precision);
					b = buf + i;
				}
				break;
			case 'u': /* Unsigned ecimal number */
				i = b - buf;
				{
					unsigned long long val;
					if (big == 2) {
						val = (unsigned long long)va_arg(args, unsigned long long);
					} else if (big == 1) {
						val = (unsigned long)va_arg(args, unsigned long);
					} else {
						val = (unsigned int)va_arg(args, unsigned int);
					}
					print_dec(val, arg_width, buf, &i, fill_zero, align, precision);
				}
				b = buf + i;
				break;
			case 'g': /* supposed to also support e */
			case 'f':
				{
					if (precision == -1) precision = 8;
					double val = (double)va_arg(args, double);
					uint64_t asBits;
					memcpy(&asBits,&val,sizeof(double));
#define SIGNBIT(d) (d & 0x8000000000000000UL)

					/* Extract exponent */
					int64_t exponent = (asBits & 0x7ff0000000000000UL) >> 52;

					/* Fraction part */
					uint64_t fraction = (asBits & 0x000fffffffffffffUL);

					if (exponent == 0x7ff) {
						if (!fraction) {
							if (SIGNBIT(asBits)) {
								*b++ = '-';
							}
							*b++ = 'i';
							*b++ = 'n';
							*b++ = 'f';
						} else {
							*b++ = 'n';
							*b++ = 'a';
							*b++ = 'n';
						}
						break;
					} else if (exponent == 0 && fraction == 0) {
						if (SIGNBIT(asBits)) {
							*b++ = '-';
						}
						*b++ = '0';
						break;
					}

					/* Okay, now we can do some real work... */

					int isNegative = !!SIGNBIT(asBits);
					if (isNegative) {
						*b++ = '-';
						val = -val;
					}

					i = b - buf;
					print_dec((unsigned long long)val, arg_width, buf, &i, fill_zero, align, 1);
					b = buf + i;
					*b++ = '.';
					i = b - buf;
					for (int j = 0; j < ((precision > -1 && precision < 16) ? precision : 16); ++j) {
						if ((unsigned long long)(val * 100000.0) % 100000 == 0 && j != 0) break;
						val = val - (unsigned long long)val;
						val *= 10.0;
						double roundy = ((double)(val - (unsigned long long)val) - 0.99999);
						if (roundy < 0.00001 && roundy > -0.00001) {
							print_dec((unsigned long long)(val) % 10 + 1, 0, buf, &i, 0, 0, 1);
							break;
						}
						print_dec((unsigned long long)(val) % 10, 0, buf, &i, 0, 0, 1);
					}
					b = buf + i;
				}
				break;
			case '%': /* Escape */
				*b++ = '%';
				break;
			default: /* Nothing at all, just dump it */
				*b++ = *f;
				break;
		}
	}
	/* Ensure the buffer ends in a null */
	*b = '\0';
	return b - buf;
}

int vasprintf(char ** buf, const char * fmt, va_list args) {
	char * b = malloc(1024);
	*buf = b;
	return xvasprintf(b, fmt, args);
}

int vsprintf(char * buf, const char *fmt, va_list args) {
	return xvasprintf(buf, fmt, args);
}

int vsnprintf(char * buf, size_t size, const char *fmt, va_list args) {
	/* XXX */
	return xvasprintf(buf, fmt, args);
}

int vfprintf(FILE * device, const char *fmt, va_list args) {
	char * buffer;
	vasprintf(&buffer, fmt, args);

	int out = fwrite(buffer, 1, strlen(buffer), device);
	free(buffer);
	return out;
}

int vprintf(const char *fmt, va_list args) {
	return vfprintf(stdout, fmt, args);
}

int fprintf(FILE * device, const char *fmt, ...) {
	va_list args;
	va_start(args, fmt);
	char * buffer;
	vasprintf(&buffer, fmt, args);
	va_end(args);

	int out = fwrite(buffer, 1, strlen(buffer), device);
	free(buffer);
	return out;
}

int printf(const char *fmt, ...) {
	va_list args;
	va_start(args, fmt);
	char * buffer;
	vasprintf(&buffer, fmt, args);
	va_end(args);
	int out = fwrite(buffer, 1, strlen(buffer), stdout);
	free(buffer);
	return out;
}

int sprintf(char * buf, const char *fmt, ...) {
	va_list args;
	va_start(args, fmt);
	int out = xvasprintf(buf, fmt, args);
	va_end(args);
	return out;
}

int snprintf(char * buf, size_t size, const char * fmt, ...) {
	/* XXX This is bad. */
	(void)size;
	va_list args;
	va_start(args, fmt);
	int out = xvasprintf(buf, fmt, args);
	va_end(args);
	return out;
}


