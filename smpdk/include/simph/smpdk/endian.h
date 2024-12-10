#ifndef __simph_smpdk_endian_h__
#define __simph_smpdk_endian_h__


/*
 * CAUTION: this way of defining the host endianness may fail with cross
 * compiler where the host running the C preprocessor has not the same
 * architecture than the target.
 */ 

#define COMPILER_HOST_LITTLE_ENDIAN 0x45545942UL
#define COMPILER_HOST_BIG_ENDIAN    0x42595445UL
#define COMPILER_HOST_BYTE_ORDER (0x42UL+0x59UL*0x100UL+0x54UL*0x10000+0x45UL*0x1000000)

#if COMPILER_HOST_BYTE_ORDER==COMPILER_HOST_LITTLE_ENDIAN
#define ntoh16(x) (((0xff00&(x))>>8) | ((0x00ff&(x))<<8))
#define ntoh32(x) (((0xff000000&(x))>>24) | ((0x00ff0000&(x))>>8) | ((0x0000ff00&(x))<<8) | ((0x000000ff&(x))<<24))
#else
#define ntoh16(x) (x)
#define ntoh32(x) (x)
#endif
#define hton16(x) ntohs(x)
#define hton32(x) ntohl(x)

#endif // __simph_smpdk_endian_h__
