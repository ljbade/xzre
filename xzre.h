/**
 * @file xzre.h
 * @author Stefano Moioli (smxdev4@gmail.com)
 * @brief XZ backdoor structures and functions
 * 
 */
#ifndef __XZRE_H
#define __XZRE_H

#include <assert.h>
#include <stddef.h>
#include <stdint.h>

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef uintptr_t uptr;

#include <lzma.h>
#include <openssl/dsa.h>
#include <openssl/ec.h>
#include <openssl/evp.h>
#include <openssl/rsa.h>
#include <elf.h>
#include <link.h>

#define UPTR(x) ((uptr)(x))
#define PTRADD(a, b) (UPTR(a) + UPTR(b))
#define PTRDIFF(a, b) (UPTR(a) - UPTR(b))

/*
 * Force a compilation error if condition is true, but also produce a
 * result (of value 0 and type int), so the expression can be used
 * e.g. in a structure initializer (or where-ever else comma expressions
 * aren't permitted).
 */
#define BUILD_BUG_ON_ZERO(e) ((int)(sizeof(struct { int:(-!!(e)); })))
#define __same_type(a, b) __builtin_types_compatible_p(typeof(a), typeof(b))
#define __must_be_array(a) BUILD_BUG_ON_ZERO(__same_type((a), &(a)[0]))
#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]) + __must_be_array(arr))

// opcode is always +0x80 for the sake of it (yet another obfuscation)
#define XZDASM_OPC(op) (op - 0x80)

typedef int BOOL;

typedef enum {
	// has lock prefix
	DF_LOCK = 1,
	// has es-segment override
	DF_ESEG = 2,
	// has operand size override
	DF_OSIZE = 4,
	// has address size override
	DF_ASIZE = 8,
	// has rex
	DF_REX = 0x20
} InstructionFlags;

typedef enum {
	// ELF has JMPREL relocs
	X_ELF_PLTREL = 0x1,
	// ELF has RELA relocs
	X_ELF_RELA = 0x2,
	// ELF has RELR relocs
	X_ELF_RELR = 0x4,
	// ELF has DT_VERSYM
	X_ELF_VERSYM = 0x10,
	// ELF has DF_1_NOW
	X_ELF_NOW = 0x20
} ElfFlags;

typedef enum {
	// register-indirect addressing or no displacement
	MRM_I_REG, // 00
	// indirect with one byte displacement
	MRM_I_DISP1, // 01
	// indirect with four byte displacement
	MRM_I_DISP4, // 10
	// direct-register addressing
	MRM_D_REG // 11
} ModRm_Mod;

typedef enum {
	// find function beginning by looking for endbr64
	FIND_ENDBR64,
	// find function beginning by looking for padding,
	// then getting the instruction after it
	FIND_NOP
} FuncFindType;

typedef enum {
	X_ELF_MAIN = 0,
	X_ELF_TMP = 1,
	X_ELF_LIBC = 2,
	X_ELF_LIBCRYPTO = 3
} ElfId;

typedef enum {
	XREF_xcalloc_zero_size = 0,
	XREF_Could_not_chdir_to_home_directory_s_s = 1,
	XREF_list_hostkey_types = 2,
	XREF_demote_sensitive_data = 3,
	XREF_mm_terminate = 4,
	XREF_mm_pty_allocate = 5,
	XREF_mm_do_pam_account = 6,
	XREF_mm_session_pty_cleanup2 = 7,
	XREF_mm_getpwnamallow = 8,
	XREF_mm_sshpam_init_ctx = 9,
	XREF_mm_sshpam_query = 10,
	XREF_mm_sshpam_respond = 11,
	XREF_mm_sshpam_free_ctx = 12,
	XREF_mm_choose_dh = 13,
	XREF_sshpam_respond = 14,
	XREF_sshpam_auth_passwd = 15,
	XREF_sshpam_query = 16,
	XREF_start_pam = 17,
	XREF_mm_request_send = 18,
	XREF_mm_log_handler = 19,
	XREF_Could_not_get_agent_socket = 20,
	XREF_auth_root_allowed = 21,
	XREF_mm_answer_authpassword = 22,
	XREF_mm_answer_keyallowed = 23,
	XREF_mm_answer_keyverify = 24,
	XREF_48s_48s_d_pid_ld_ = 25,
	XREF_Unrecognized_internal_syslog_level_code_d = 26
} StringXrefId;

typedef enum {
	STR_from = 0x810,
	STR_ssh2 = 0x678,
	STR_48s_48s_d_pid_ld_ = 0xd8,
	STR_s = 0x708,
	STR_usr_sbin_sshd = 0x108,
	STR_Accepted_password_for = 0x870,
	STR_Accepted_publickey_for = 0x1a0,
	STR_BN_bin2bn = 0xc40,
	STR_BN_bn2bin = 0x6d0,
	STR_BN_dup = 0x958,
	STR_BN_free = 0x418,
	STR_BN_num_bits = 0x4e0,
	STR_Connection_closed_by = 0x790,
	STR_Could_not_chdir_to_home_directory_s_s = 0x18,
	STR_Could_not_get_agent_socket = 0xb0,
	STR_DISPLAY = 0x960,
	STR_DSA_get0_pqg = 0x9d0,
	STR_DSA_get0_pub_key = 0x468,
	STR_EC_KEY_get0_group = 0x7e8,
	STR_EC_KEY_get0_public_key = 0x268,
	STR_EC_POINT_point2oct = 0x6e0,
	STR_EVP_CIPHER_CTX_free = 0xb28,
	STR_EVP_CIPHER_CTX_new = 0x838,
	STR_EVP_DecryptFinal_ex = 0x2a8,
	STR_EVP_DecryptInit_ex = 0xc08,
	STR_EVP_DecryptUpdate = 0x3f0,
	STR_EVP_Digest = 0xf8,
	STR_EVP_DigestVerify = 0x408,
	STR_EVP_DigestVerifyInit = 0x118,
	STR_EVP_MD_CTX_free = 0xd10,
	STR_EVP_MD_CTX_new = 0xaf8,
	STR_EVP_PKEY_free = 0x6f8,
	STR_EVP_PKEY_new_raw_public_key = 0x758,
	STR_EVP_PKEY_set1_RSA = 0x510,
	STR_EVP_chacha20 = 0xc28,
	STR_EVP_sha256 = 0xc60,
	STR_EVP_sm = 0x188,
	STR_GLIBC_2_2_5 = 0x8c0,
	STR_GLRO_dl_naudit_naudit = 0x6a8,
	STR_KRB5CCNAME = 0x1e0,
	STR_LD_AUDIT = 0xcf0,
	STR_LD_BIND_NOT = 0xbc0,
	STR_LD_DEBUG = 0xa90,
	STR_LD_PROFILE = 0xb98,
	STR_LD_USE_LOAD_BIAS = 0x3e0,
	STR_LINES = 0xa88,
	STR_RSA_free = 0xac0,
	STR_RSA_get0_key = 0x798,
	STR_RSA_new = 0x918,
	STR_RSA_public_decrypt = 0x1d0,
	STR_RSA_set0_key = 0x540,
	STR_RSA_sign = 0x8f8,
	STR_SSH_2_0 = 0x990,
	STR_TERM = 0x4a8,
	STR_Unrecognized_internal_syslog_level_code_d = 0xe0,
	STR_WAYLAND_DISPLAY = 0x158,
	STR_errno_location = 0x878,
	STR_libc_stack_end = 0x2b0,
	STR_libc_start_main = 0x228,
	STR_dl_audit_preinit = 0xa60,
	STR_dl_audit_symbind_alt = 0x9c8,
	STR_exit = 0x8a8,
	STR_r_debug = 0x5b0,
	STR_rtld_global = 0x5b8,
	STR_rtld_global_ro = 0xa98,
	STR_auth_root_allowed = 0xb8,
	STR_authenticating = 0x1d8,
	STR_demote_sensitive_data = 0x28,
	STR_getuid = 0x348,
	STR_ld_linux_x86_64_so = 0xa48,
	STR_libc_so = 0x7d0,
	STR_libcrypto_so = 0x7c0,
	STR_liblzma_so = 0x590,
	STR_libsystemd_so = 0x938,
	STR_list_hostkey_types = 0x20,
	STR_malloc_usable_size = 0x440,
	STR_mm_answer_authpassword = 0xc0,
	STR_mm_answer_keyallowed = 0xc8,
	STR_mm_answer_keyverify = 0xd0,
	STR_mm_answer_pam_start = 0x948,
	STR_mm_choose_dh = 0x78,
	STR_mm_do_pam_account = 0x40,
	STR_mm_getpwnamallow = 0x50,
	STR_mm_log_handler = 0xa8,
	STR_mm_pty_allocate = 0x38,
	STR_mm_request_send = 0xa0,
	STR_mm_session_pty_cleanup2 = 0x48,
	STR_mm_sshpam_free_ctx = 0x70,
	STR_mm_sshpam_init_ctx = 0x58,
	STR_mm_sshpam_query = 0x60,
	STR_mm_sshpam_respond = 0x68,
	STR_mm_terminate = 0x30,
	STR_parse_PAM = 0xc58,
	STR_password = 0x400,
	STR_preauth = 0x4f0,
	STR_pselect = 0x690,
	STR_publickey = 0x7b8,
	STR_read = 0x308,
	STR_rsa_sha2_256 = 0x710,
	STR_setlogmask = 0x428,
	STR_setresgid = 0x5f0,
	STR_setresuid = 0xab8,
	STR_shutdown = 0x760,
	STR_ssh_2_0 = 0xd08,
	STR_ssh_rsa_cert_v01_openssh_com = 0x2c8,
	STR_sshpam_auth_passwd = 0x88,
	STR_sshpam_query = 0x90,
	STR_sshpam_respond = 0x80,
	STR_start_pam = 0x98,
	STR_system = 0x9f8,
	STR_unknown = 0x198,
	STR_user = 0xb10,
	STR_write = 0x380,
	STR_xcalloc_zero_size = 0x10,
	STR_yolAbejyiejuvnupEvjtgvsh5okmkAvj = 0xb00,
	STR_ELF = 0x300,
} EncodedStringId;


#define assert_offset(t, f, o) static_assert(offsetof(t, f) == o)

#define CONCAT(x, y) x ## y
#define EXPAND(x, y) CONCAT(x, y)
#define PADDING(size) u8 EXPAND(_unknown, __LINE__)[size]

typedef struct __attribute__((packed)) {
	/**
	 * @brief points to a symbol in memory
	 * will be used to find the GOT value
	 */
	void *symbol_ptr;
	/**
	 * @brief points to the Global Offset Table
	 */
	void *got_ptr;
	/**
	 * @brief the return address value of the caller
	 * obtained from *(u64 *)(caller_locals+24)
	 * since the entrypoint passes __builtin_frame_address(0)-16,
	 * this results in an offset of +8
	 */
	void *return_address;
	/**
	 * @brief points to the real cpuid function
	 */
	void *cpuid_fn;
	/**
	 * @brief holds the offset of the symbol relative to the GOT.
	 * used to derive the @ref got_ptr
	 */
	u64 got_offset;
	/**
	 * @brief stores the value of __builtin_frame_address(0)-16
	 */
	u64 *frame_address;
} elf_entry_ctx_t;

assert_offset(elf_entry_ctx_t, symbol_ptr, 0);
assert_offset(elf_entry_ctx_t, got_ptr, 8);
assert_offset(elf_entry_ctx_t, return_address, 0x10);
assert_offset(elf_entry_ctx_t, cpuid_fn, 0x18);
assert_offset(elf_entry_ctx_t, got_offset, 0x20);
assert_offset(elf_entry_ctx_t, frame_address, 0x28);

typedef struct __attribute__((packed)) {
	u8* instruction;
	u64 instruction_size;
	u8 flags;
	u8 flags2;
	PADDING(2);
	u8 lock_byte;
	u8 _unk1;
	u8 last_prefix;
	PADDING(4);
	u8 rex_byte;
	u8 modrm;
	u8 modrm_mod;
	u8 modrm_reg;
	u8 modrm_rm;
	PADDING(4);
	u8 byte_24;
	PADDING(3);
	u32 opcode;
	PADDING(4);
	u64 mem_disp;
	// e.g. in CALL
	u64 operand;
	PADDING(16);
	u8 insn_offset;
	PADDING(47);
} dasm_ctx_t;

assert_offset(dasm_ctx_t, instruction, 0);
assert_offset(dasm_ctx_t, instruction_size, 8);
assert_offset(dasm_ctx_t, flags, 0x10);
assert_offset(dasm_ctx_t, flags2, 0x11);
assert_offset(dasm_ctx_t, lock_byte, 0x14);
assert_offset(dasm_ctx_t, last_prefix, 0x16);
assert_offset(dasm_ctx_t, rex_byte, 0x1B);
assert_offset(dasm_ctx_t, modrm, 0x1C);
assert_offset(dasm_ctx_t, modrm_mod, 0x1D);
assert_offset(dasm_ctx_t, modrm_reg, 0x1E);
assert_offset(dasm_ctx_t, modrm_rm, 0x1F);
assert_offset(dasm_ctx_t, opcode, 0x28);
assert_offset(dasm_ctx_t, mem_disp, 0x30);
assert_offset(dasm_ctx_t, operand, 0x38);
assert_offset(dasm_ctx_t, insn_offset, 0x50);
static_assert(sizeof(dasm_ctx_t) == 128);

typedef struct __attribute__((packed)) elf_info {
	/**
	 * @brief pointed to the ELF base address in memory
	 */
	Elf64_Ehdr *elfbase;
	/**
	 * @brief virtual address of the first program header
	 */
	u64 first_vaddr;
	/**
	 * @brief pointer to the ELF program headers array in memory
	 */
	Elf64_Phdr *phdrs;
	/**
	 * @brief copy of the ELF program header count from the ELF header
	 */
	u64 e_phnum;
	/**
	 * @brief pointer to the ELF dynamic segment
	 */
	Elf64_Dyn *dyn;
	/**
	 * @brief number of entries in the ELF dynamic segment
	 */
	u64 dyn_num_entries;
	/**
	 * @brief pointer to the ELF string table
	 */
	char *strtab;
	/**
	 * @brief pointer to the ELF symbol table
	 */
	Elf64_Sym *symtab;
	/**
	 * @brief pointer to the ELF PLT relocations table
	 */
	Elf64_Rela *plt_relocs;
	/**
	 * @brief number of entries in the PLT relocation table
	 */
	u32 plt_relocs_num;
	/**
	 * @brief whether the loaded ELF contains PT_GNU_RELRO or not
	 * which specifies the location and size of a segment which
	 * may be made read-only after relocations have been processed.
	 */
	BOOL gnurelro_found;
	/**
	 * @brief location of the GNU relro segment
	 */
	u64 gnurelro_vaddr;
	/**
	 * @brief size of the GNU relro segment
	 */
	u64 gnurelro_memsize;
	/**
	 * @brief pointer to the EFL symbol versioning  (from DT_VERDEF)
	 */
	Elf64_Verdef *verdef;
	/**
	 * @brief number of entries in the symbol versioning table
	 */
	u64 verdef_num;
	Elf64_Versym *versym;
	Elf64_Rela *rela_relocs;
	u32 rela_relocs_num;
	u32 _unused0;
	Elf64_Relr *relr_relocs;
	u32 relr_relocs_num;
	PADDING(4);
	/**
	 * @brief
	 * page-aligned virtual address of the first executable ELF segment
	 */
	u64 code_segment_start;
	/**
	 * @brief 
	 * page-aligned virtual size of the first executable ELF segment
	 */
	u64 code_segment_size;

	u64 rodata_segment_start;
	u64 rodata_segment_size;
	u64 data_segment_start;
	u64 data_segment_size;
	u64 data_segment_alignment;

	u8 flags;
	PADDING(7);
	/**
	 * @brief number of GNU hash buckets (from DT_GNU_HASH)
	 */
	u32 gnu_hash_nbuckets;
	/**
	 * @brief last valid bloom value
	 */
	u32 gnu_hash_last_bloom;
	u32 gnu_hash_bloom_shift;
	PADDING(4);
	u64 *gnu_hash_bloom;
	u32 *gnu_hash_buckets;
	u32 *gnu_hash_chain;
} elf_info_t;

assert_offset(elf_info_t, elfbase, 0x0);
assert_offset(elf_info_t, first_vaddr, 0x8);
assert_offset(elf_info_t, phdrs, 0x10);
assert_offset(elf_info_t, e_phnum, 0x18);
assert_offset(elf_info_t, dyn, 0x20);
assert_offset(elf_info_t, dyn_num_entries, 0x28);
assert_offset(elf_info_t, strtab, 0x30);
assert_offset(elf_info_t, symtab, 0x38);
assert_offset(elf_info_t, plt_relocs, 0x40);
assert_offset(elf_info_t, plt_relocs_num, 0x48);
assert_offset(elf_info_t, gnurelro_found, 0x4C);
assert_offset(elf_info_t, gnurelro_vaddr, 0x50);
assert_offset(elf_info_t, gnurelro_memsize, 0x58);
assert_offset(elf_info_t, verdef, 0x60);
assert_offset(elf_info_t, verdef_num, 0x68);
assert_offset(elf_info_t, versym, 0x70);
assert_offset(elf_info_t, rela_relocs, 0x78);
assert_offset(elf_info_t, rela_relocs_num, 0x80);
assert_offset(elf_info_t, relr_relocs, 0x88);
assert_offset(elf_info_t, relr_relocs_num, 0x90);
assert_offset(elf_info_t, code_segment_start, 0x98);
assert_offset(elf_info_t, code_segment_size, 0xA0);
assert_offset(elf_info_t, flags, 0xD0);
assert_offset(elf_info_t, gnu_hash_nbuckets, 0xd8);
assert_offset(elf_info_t, gnu_hash_last_bloom, 0xdc);
assert_offset(elf_info_t, gnu_hash_bloom_shift, 0xe0);
assert_offset(elf_info_t, gnu_hash_bloom, 0xe8);
assert_offset(elf_info_t, gnu_hash_buckets, 0xf0);
assert_offset(elf_info_t, gnu_hash_chain, 0xf8);

typedef struct __attribute__((packed)) {
	u32 resolved_imports_count;
	PADDING(4);
	size_t (*malloc_usable_size)(void *ptr);
	uid_t (*getuid)(void);
	void (*exit)(int status);
	int (*setresgid)(gid_t rgid, gid_t egid, gid_t sgid); 
	int (*setresuid)(uid_t ruid, uid_t euid, uid_t suid);
	int (*system)(const char *command);
	ssize_t (*write)(int fd, const void *buf, size_t count);
	int (*pselect)(
		int nfds, fd_set *readfds, fd_set *writefds,
		fd_set *exceptfds, const struct timespec *timeout,
		const sigset_t *sigmask);
	ssize_t (*read)(int fd, void *buf, size_t count);
	int *(*__errno_location)(void);
	int (*setlogmask)(int mask);
	int (*shutdown)(int sockfd, int how);
	void **libc_stack_end;
} libc_imports_t;

assert_offset(libc_imports_t, resolved_imports_count, 0);
assert_offset(libc_imports_t, malloc_usable_size, 8);
assert_offset(libc_imports_t, getuid, 0x10);
assert_offset(libc_imports_t, exit, 0x18);
assert_offset(libc_imports_t, setresgid, 0x20);
assert_offset(libc_imports_t, setresuid, 0x28);
assert_offset(libc_imports_t, system, 0x30);
assert_offset(libc_imports_t, write, 0x38);
assert_offset(libc_imports_t, pselect, 0x40);
assert_offset(libc_imports_t, read, 0x48);
assert_offset(libc_imports_t, __errno_location, 0x50);
assert_offset(libc_imports_t, setlogmask, 0x58);
assert_offset(libc_imports_t, shutdown, 0x60);
static_assert(sizeof(libc_imports_t) == 0x70);

typedef int (*pfn_RSA_public_decrypt_t)(
	int flen, unsigned char *from, unsigned char *to,
	RSA *rsa, int padding);
typedef int (*pfn_EVP_PKEY_set1_RSA_t)(EVP_PKEY *pkey, struct rsa_st *key);
typedef void (*pfn_RSA_get0_key_t)(
	const RSA *r,
	const BIGNUM **n, const BIGNUM **e, const BIGNUM **d);

typedef struct __attribute__((packed)) {
	pfn_RSA_public_decrypt_t RSA_public_decrypt;
	pfn_EVP_PKEY_set1_RSA_t EVP_PKEY_set1_RSA;
	// ???
	void (*RSA_get0_key_null)(
		const RSA *r, const BIGNUM **n,
		const BIGNUM **e, const BIGNUM **d);
	void *RSA_public_decrypt_hook_ptr;
	void *EVP_PKEY_set1_RSA_hook_ptr;
	void *RSA_get0_key_hook_ptr;
	void (*DSA_get0_pqg)(
		const DSA *d, const BIGNUM **p,
		const BIGNUM **q, const BIGNUM **g);
	const BIGNUM *(*DSA_get0_pub_key)(const DSA *d);
	size_t (*EC_POINT_point2oct)(
		const EC_GROUP *group, const EC_POINT *p,
		point_conversion_form_t form, unsigned char *buf,
		size_t len, BN_CTX *ctx);
	EC_POINT *(*EC_KEY_get0_public_key)(const EC_KEY *key);
	const EC_GROUP *(*EC_KEY_get0_group)(const EC_KEY *key);
	EVP_MD *(*EVP_sha256)(void);
	pfn_RSA_get0_key_t RSA_get0_key;
	int (*BN_num_bits)(const BIGNUM *a);
	EVP_PKEY *(*EVP_PKEY_new_raw_public_key)(
		int type, ENGINE *e,
		const unsigned char *key, size_t keylen);
	EVP_MD_CTX *(*EVP_MD_CTX_new)(void);
	int (*EVP_DigestVerifyInit)(
		EVP_MD_CTX *ctx, EVP_PKEY_CTX **pctx,
		const EVP_MD *type, ENGINE *e, EVP_PKEY *pkey);
	int (*EVP_DigestVerify)(
		EVP_MD_CTX *ctx, const unsigned char *sig,
		size_t siglen, const unsigned char *tbs, size_t tbslen);
	void (*EVP_MD_CTX_free)(EVP_MD_CTX *ctx);
	void (*EVP_PKEY_free)(EVP_PKEY *key);
	EVP_CIPHER_CTX *(*EVP_CIPHER_CTX_new)(void);
	int (*EVP_DecryptInit_ex)(
		EVP_CIPHER_CTX *ctx, const EVP_CIPHER *type,
		ENGINE *impl, const unsigned char *key, const unsigned char *iv);
	int (*EVP_DecryptUpdate)(
		EVP_CIPHER_CTX *ctx, unsigned char *out,
		int *outl, const unsigned char *in, int inl);
	int (*EVP_DecryptFinal_ex)(EVP_CIPHER_CTX *ctx, unsigned char *outm, int *outl);
	void (*EVP_CIPHER_CTX_free)(EVP_CIPHER_CTX *ctx);
	EVP_CIPHER *(*EVP_chacha20)(void);
	RSA *(*RSA_new)(void);
	BIGNUM *(*BN_dup)(const BIGNUM *from);
	BIGNUM (*BN_bin2bn)(const unsigned char *s, int len, BIGNUM *ret);
	int (*RSA_set0_key)(RSA *r, BIGNUM *n, BIGNUM *e, BIGNUM *d);
	int (*EVP_Digest)(
		const void *data, size_t count, unsigned char *md,
		unsigned int *size, const EVP_MD *type, ENGINE *impl);
	int (*RSA_sign)(
		int type,
		const unsigned char *m, unsigned int m_len,
		unsigned char *sigret, unsigned int *siglen, RSA *rsa);
	int (*BN_bn2bin)(const BIGNUM *a, unsigned char *to);
	void (*RSA_free)(RSA *rsa);
	void (*BN_free)(BIGNUM *a);
	libc_imports_t *libc;
	u64 resolved_imports_count;
} imported_funcs_t;

assert_offset(imported_funcs_t, RSA_public_decrypt, 0);
assert_offset(imported_funcs_t, EVP_PKEY_set1_RSA, 8);
assert_offset(imported_funcs_t, RSA_get0_key_null, 0x10);
assert_offset(imported_funcs_t, RSA_public_decrypt_hook_ptr, 0x18);
assert_offset(imported_funcs_t, EVP_PKEY_set1_RSA_hook_ptr, 0x20);
assert_offset(imported_funcs_t, RSA_get0_key_hook_ptr, 0x28);
assert_offset(imported_funcs_t, DSA_get0_pqg, 0x30);
assert_offset(imported_funcs_t, DSA_get0_pub_key, 0x38);
assert_offset(imported_funcs_t, EC_POINT_point2oct, 0x40);
assert_offset(imported_funcs_t, EC_KEY_get0_public_key, 0x48);
assert_offset(imported_funcs_t, EC_KEY_get0_group, 0x50);
assert_offset(imported_funcs_t, EVP_sha256, 0x58);
assert_offset(imported_funcs_t, RSA_get0_key, 0x60);
assert_offset(imported_funcs_t, BN_num_bits, 0x68);
assert_offset(imported_funcs_t, EVP_PKEY_new_raw_public_key, 0x70);
assert_offset(imported_funcs_t, EVP_MD_CTX_new, 0x78);
assert_offset(imported_funcs_t, EVP_DigestVerifyInit, 0x80);
assert_offset(imported_funcs_t, EVP_DigestVerify, 0x88);
assert_offset(imported_funcs_t, EVP_MD_CTX_free, 0x90);
assert_offset(imported_funcs_t, EVP_PKEY_free, 0x98);
assert_offset(imported_funcs_t, EVP_CIPHER_CTX_new, 0xA0);
assert_offset(imported_funcs_t, EVP_DecryptInit_ex, 0xA8);
assert_offset(imported_funcs_t, EVP_DecryptUpdate, 0xB0);
assert_offset(imported_funcs_t, EVP_DecryptFinal_ex, 0xB8);
assert_offset(imported_funcs_t, EVP_CIPHER_CTX_free, 0xC0);
assert_offset(imported_funcs_t, EVP_chacha20, 0xC8);
assert_offset(imported_funcs_t, RSA_new, 0xD0);
assert_offset(imported_funcs_t, BN_dup, 0xD8);
assert_offset(imported_funcs_t, BN_bin2bn, 0xE0);
assert_offset(imported_funcs_t, RSA_set0_key, 0xE8);
assert_offset(imported_funcs_t, EVP_Digest, 0xF0);
assert_offset(imported_funcs_t, RSA_sign, 0xF8);
assert_offset(imported_funcs_t, BN_bn2bin, 0x100);
assert_offset(imported_funcs_t, RSA_free, 0x108);
assert_offset(imported_funcs_t, BN_free, 0x110);
assert_offset(imported_funcs_t, libc, 0x118);
assert_offset(imported_funcs_t, resolved_imports_count, 0x120);

typedef struct __attribute__((packed)) {
	PADDING(8);
	/**
	 * @brief 
	 * pointer to the structure containing resolved OpenSSL and system functions
	 */
	imported_funcs_t *imported_funcs;
	PADDING(0x70);
	/**
	 * @brief 
	 * the shifter will use this address as the minimum search address
	 * any instruction below this address will be rejected
	 */
	u64 code_range_start;
	/**
	 * @brief 
	 * the shifter will use this address as the maximum search address
	 * any instruction beyond this address will be rejected
	 */
	u64 code_range_end;
	PADDING(0x78);
	/**
	 * @brief 
	 * holds the secret data used for the chacha key generation
	 */
	u8 secret_data[57];
	/**
	 * @brief
	 * holds the shift operation states
	 * written by @ref secret_data_append_singleton
	 */
	u8 shift_operations[31];
	/**
	 * @brief 
	 * number of bits copied
	 */
	u32 num_shifted_bits;
	PADDING(4);
} global_context_t;

assert_offset(global_context_t, imported_funcs, 8);
assert_offset(global_context_t, code_range_start, 0x80);
assert_offset(global_context_t, code_range_end, 0x88);
assert_offset(global_context_t, secret_data, 0x108);
assert_offset(global_context_t, shift_operations, 0x141);
assert_offset(global_context_t, num_shifted_bits, 0x160);
static_assert(sizeof(global_context_t) == 0x168);

typedef struct __attribute__((packed)) {
	PADDING(0x10);
	global_context_t *globals;
} backdoor_shared_globals_t;

assert_offset(backdoor_shared_globals_t, globals, 0x10);

typedef struct __attribute__((packed)) {
	PADDING(0x70);
	PADDING(sizeof(void *));
	PADDING(sizeof(u64));
	PADDING(0x78);
	PADDING(sizeof(u64));
	void *dl_audit_symbind_alt;
	u64 dl_audit_symbind_alt__size;
	typeof(&RSA_public_decrypt) hook_RSA_public_decrypt;
	typeof(&EVP_PKEY_set1_RSA) hook_EVP_PKEY_set1_RSA;
	typeof(&RSA_get0_key) hook_RSA_get0_key;
	imported_funcs_t *imports;
	u64 hooks_installed;
} ldso_ctx_t;

assert_offset(ldso_ctx_t, dl_audit_symbind_alt, 0x100);
assert_offset(ldso_ctx_t, dl_audit_symbind_alt__size, 0x108);
assert_offset(ldso_ctx_t, hook_RSA_public_decrypt, 0x110);
assert_offset(ldso_ctx_t, hook_EVP_PKEY_set1_RSA, 0x118);
assert_offset(ldso_ctx_t, hook_RSA_get0_key, 0x120);
static_assert(sizeof(ldso_ctx_t) == 0x138);


typedef struct __attribute__((packed)) {
	ldso_ctx_t ldso_ctx;
	global_context_t global_ctx;
	imported_funcs_t imported_funcs;
	PADDING(0xE0);
	libc_imports_t libc_imports;
	PADDING(0x70);
} backdoor_hooks_data_t;

assert_offset(backdoor_hooks_data_t, ldso_ctx, 0);
assert_offset(backdoor_hooks_data_t, global_ctx, 0x138);
assert_offset(backdoor_hooks_data_t, imported_funcs, 0x2A0);
assert_offset(backdoor_hooks_data_t, libc_imports, 0x4A8);
static_assert(sizeof(backdoor_hooks_data_t) == 0x588);

typedef struct __attribute__((packed)) {
	PADDING(0x30);
	backdoor_shared_globals_t *shared;
	backdoor_hooks_data_t **hooks_data_addr;
	uintptr_t (*symbind64)(
		Elf64_Sym *sym, unsigned int ndx,
		uptr *refcook, uptr *defcook,
		unsigned int flags, const char *symname);
	typeof(&RSA_public_decrypt) hook_RSA_public_decrypt;
	typeof(&RSA_get0_key) hook_RSA_get0_key;
	PADDING(sizeof(void *));
	PADDING(sizeof(void *));
	PADDING(sizeof(void *));
	PADDING(sizeof(void *));
	PADDING(sizeof(void *));
	PADDING(sizeof(void *));
} backdoor_hooks_ctx_t;

assert_offset(backdoor_hooks_ctx_t, shared, 0x30);
assert_offset(backdoor_hooks_ctx_t, hooks_data_addr, 0x38);
assert_offset(backdoor_hooks_ctx_t, symbind64, 0x40);
assert_offset(backdoor_hooks_ctx_t, hook_RSA_public_decrypt, 0x48);
assert_offset(backdoor_hooks_ctx_t, hook_RSA_get0_key, 0x50);

static_assert(sizeof(backdoor_hooks_ctx_t) == 0x88);


typedef struct __attribute__((packed)) {
	PADDING(0x8);
	backdoor_shared_globals_t *shared;
	backdoor_hooks_ctx_t *hook_params;
	PADDING(0x68);
	elf_entry_ctx_t *entry_ctx;
} backdoor_setup_params_t;

assert_offset(backdoor_setup_params_t, shared, 0x8);
assert_offset(backdoor_setup_params_t, entry_ctx, 0x80);
static_assert(sizeof(backdoor_setup_params_t) == 0x88);

/**
 * @brief array of ELF handles
 * @see ElfId maps the indices
 */
typedef struct __attribute__((packed)) {
	elf_info_t *main;
	/**
	 * @brief used for multiple ELFs
	 */
	elf_info_t *tmp;
	elf_info_t *libc;
	elf_info_t *liblzma;
	elf_info_t *libcrypto;
} elf_handles_t;

assert_offset(elf_handles_t, main, 0x0);
assert_offset(elf_handles_t, tmp, 0x8);
assert_offset(elf_handles_t, libc, 0x10);
assert_offset(elf_handles_t, liblzma, 0x18);
assert_offset(elf_handles_t, libcrypto, 0x20);

typedef struct __attribute__((packed)) {
	elf_handles_t *handles;
	Elf64_Ehdr *ehdr;
	void *__libc_stack_end;
} main_elf_t;

assert_offset(main_elf_t, handles, 0x0);
assert_offset(main_elf_t, ehdr, 0x8);
assert_offset(main_elf_t, __libc_stack_end, 0x10);


struct backdoor_data;

/**
 * @brief data passed to functions that access the backdoor data
 */
typedef struct __attribute__((packed)) {
	struct backdoor_data *data;
	elf_handles_t *elf_handles;
} backdoor_data_handle_t;

assert_offset(backdoor_data_handle_t, data, 0x0);
assert_offset(backdoor_data_handle_t, elf_handles, 0x8);


/**
 * @brief this structure is used to hold most of the backdoor information.
 * it's used as a local variable in function @ref backdoor_setup
 */
typedef struct __attribute__((packed)) backdoor_data {
	struct link_map *main_map;
	struct link_map *dynamic_linker_map;
	struct link_map *liblzma_map;
	struct link_map *libcrypto_map;
	struct link_map *libsystemd_map;
	struct link_map *libc_map;

	elf_handles_t elf_handles;

	/**
	 * @brief points to the beginning of this struct
	 */
	struct backdoor_data *backdoor_data;
	PADDING(sizeof(elf_handles_t *));

	/** parsed ELF files */
	PADDING(sizeof(elf_info_t));
	PADDING(sizeof(elf_info_t));
	/**
	 * @brief ELF context for libc.so
	 */
	elf_info_t libc_info;
	PADDING(sizeof(elf_info_t));
	/**
	 * @brief ELF context for libcrypto.so
	 */
	elf_info_t libcrypto_info;

	/**
	 * @brief functions imported from libc
	 */
	libc_imports_t libc_imports;

	PADDING(0x378);
	/**
	 * @brief ELF import resolver (fake LZMA allocator)
	 */
	lzma_allocator *import_resolver;
} backdoor_data_t;

assert_offset(backdoor_data_t, main_map, 0);
assert_offset(backdoor_data_t, dynamic_linker_map, 0x8);
assert_offset(backdoor_data_t, liblzma_map, 0x10);
assert_offset(backdoor_data_t, libcrypto_map, 0x18);
assert_offset(backdoor_data_t, libsystemd_map, 0x20);
assert_offset(backdoor_data_t, libc_map, 0x28);
assert_offset(backdoor_data_t, elf_handles, 0x30);
assert_offset(backdoor_data_t, libc_info, 0x268);
assert_offset(backdoor_data_t, libcrypto_info, 0x468);
assert_offset(backdoor_data_t, libc_imports, 0x568);
assert_offset(backdoor_data_t, import_resolver, 0x950);
static_assert(sizeof(backdoor_data_t) == 0x958);

typedef struct __attribute__((packed)) {
	backdoor_data_t *data;
	elf_handles_t *elf_handles;
	pfn_RSA_public_decrypt_t RSA_public_decrypt;
	pfn_EVP_PKEY_set1_RSA_t EVP_PKEY_set1_RSA;
	pfn_RSA_get0_key_t RSA_get0_key;
	PADDING(sizeof(void *));
	libc_imports_t *libc_imports;
} backdoor_shared_libraries_data_t;

assert_offset(backdoor_shared_libraries_data_t, data, 0x0);
assert_offset(backdoor_shared_libraries_data_t, elf_handles, 0x8);
assert_offset(backdoor_shared_libraries_data_t, RSA_public_decrypt, 0x10);
assert_offset(backdoor_shared_libraries_data_t, EVP_PKEY_set1_RSA, 0x18);
assert_offset(backdoor_shared_libraries_data_t, RSA_get0_key, 0x20);
assert_offset(backdoor_shared_libraries_data_t, libc_imports, 0x30);

/**
 * @brief represents a shift register, which will shift 
 * a '1' into the secret data array.
 * the low 3 bits represent the bit index, while the rest represents the byte index
 * this is convenient, since a simple increment will increment the buffer position correctly
 */
typedef union {
	/** the initial value */
	u32 index;
	struct {
		/** bit index in the current byte indicated by @ref byte_index */
		u32 bit_index : 3;
		/** byte index into the secret data array */
		u32 byte_index : 29;
	};
} secret_data_shift_cursor;

typedef struct __attribute__((packed)) {
	/**
	 * @brief the string that was referenced, in encoded form
	 */
	EncodedStringId string_id;
	PADDING(4);
	/**
	 * @brief the starting address of the function that referenced the string
	 */
	void *func_start;
	/**
	 * @brief the ending address of the function that referenced the string
	 */
	void *func_end;
	/**
	 * @brief location of the instruction that referenced the string
	 */
	void *xref;
} string_item_t;

assert_offset(string_item_t, string_id, 0);
assert_offset(string_item_t, func_start, 0x8);
assert_offset(string_item_t, func_end, 0x10);
assert_offset(string_item_t, xref, 0x18);
static_assert(sizeof(string_item_t) == 0x20);

typedef struct __attribute__((packed)) {
	string_item_t entries[27];
	PADDING(0x8);
} string_references_t;

assert_offset(string_references_t, entries, 0);

/**
 * @brief the payload header. also used as Chacha IV
 * 
 * @return typedef struct 
 */
typedef struct __attribute__((packed)) {
	PADDING(0x4);
	PADDING(0x4);
	PADDING(0x8);
} key_payload_hdr_t;

typedef struct __attribute__((packed)) {
	PADDING(0x218);
} key_payload_body_t;

/**
 * @brief the contents of the RSA 'n' field
 * 
 * @return typedef struct 
 */
typedef struct __attribute__((packed)) {
	key_payload_hdr_t header;
	key_payload_body_t body;
} key_payload_t;

typedef union __attribute__((packed)) {
	u8 value[2];
	u16 size;
} u_cmd_arguments;

enum CommandFlags1 {
	/**
	 * @brief the data block contains 8 additional bytes
	 */
	CMDF_8BYTES = 1 << 0,
	/**
	 * @brief disable all logging by setting mask 0x80000000
	 */
	CMDF_SETLOGMASK = 1 << 2
};

enum CommandFlags2 {
	/**
	 * @brief more data available in the following packet
	 * not compatible with command 3
	 */
	CMDF_CONTINUATION = 0x40,
	/**
	 * @brief executes pselect, then exit
	 * not compatible with command 2
	 */
	CMDF_PSELECT = 0xC0
};

typedef struct __attribute__((packed)) {
	u8 flags1;
	u8 flags2;
	u8 flags3;
	u_cmd_arguments u;
} cmd_arguments_t;

assert_offset(cmd_arguments_t, flags1, 0);
assert_offset(cmd_arguments_t, flags2, 1);
assert_offset(cmd_arguments_t, flags3, 2);
assert_offset(cmd_arguments_t, u, 3);
static_assert(sizeof(cmd_arguments_t) == 0x5);

typedef struct __attribute__((packed)) {
	BIGNUM *rsa_n;
	BIGNUM *rsa_e;
	cmd_arguments_t args;
	key_payload_t payload;
	PADDING(0x30);
	PADDING(sizeof(key_payload_hdr_t));
	/**
	 * @brief ChaCha Key
	 */
	u8 decrypted_secret_data[57];
	PADDING(2);
} key_ctx_t;

assert_offset(key_ctx_t, rsa_n, 0);
assert_offset(key_ctx_t, rsa_e, 0x8);
assert_offset(key_ctx_t, args, 0x10);
assert_offset(key_ctx_t, payload, 0x15);
static_assert(sizeof(key_ctx_t) == 0x2B8);

/**
 * @brief disassembles the given x64 code
 *
 * @param ctx empty disassembler context to hold the state
 * @param code_start pointer to the start of buffer (first disassemblable location)
 * @param code_end pointer to the end of the buffer
 * @return int TRUE if disassembly was successful, FALSE otherwise
 */
extern int x86_dasm(dasm_ctx_t *ctx, u8 *code_start, u8 *code_end);

/**
 * @brief finds a call instruction
 *
 * @param code_start address to start searching from
 * @param code_end address to stop searching at
 * @param call_target optional call target address. pass 0 to find any call
 * @param dctx empty disassembler context to hold the state
 * @return BOOL TRUE if found, FALSE otherwise
 */
extern BOOL find_call_instruction(u8 *code_start, u8 *code_end, u8 *call_target, dasm_ctx_t *dctx);

/**
 * @brief finds a lea instruction
 * 
 * @param code_start address to start searching from
 * @param code_end address to stop searching at
 * @param displacement the memory displacement operand of the target lea instruction
 * @return BOOL TRUE if found, FALSE otherwise
 */
extern BOOL find_lea_instruction(u8 *code_start, u8 *code_end, u64 displacement);

/**
 * @brief finds a LEA or MOV instruction with an immediate memory operand
 * 
 * @param code_start address to start searching from
 * @param code_end address to stop searching at
 * @param dctx disassembler context to hold the state
 * @param mem_address the address of the memory fetch (where the instruction will fetch from)
 * @return BOOL TRUE if an instruction was found, FALSE otherwise
 */
extern BOOL find_instruction_with_mem_operand(
	u8 *code_start,
	u8 *code_end,
	dasm_ctx_t *dctx,
	void *mem_address
);

/**
 * @brief finds a LEA instruction with an immediate memory operand
 * 
 * @param code_start address to start searching from
 * @param code_end address to stop searching at
 * @param dctx disassembler context to hold the state
 * @param mem_address the expected address of the memory access
 * @return BOOL TRUE if found, FALSE otherwise
 */
extern BOOL find_lea_instruction_with_mem_operand(
	u8 *code_start,
	u8 *code_end,
	dasm_ctx_t *dctx,
	void *mem_address
);

/**
 * @brief finds an instruction with an immediate memory operand
 * 
 * @param code_start address to start searching from
 * @param code_end address to stop searching at
 * @param dctx disassembler context to hold the state
 * @param opcode opcode to look for, in encoded form (+0x80)
 * @param mem_address the expected address of the memory access
 * @return BOOL TRUE if found, FALSE otherwise
 */
extern BOOL find_instruction_with_mem_operand_ex(
	u8 *code_start,
	u8 *code_end,
	dasm_ctx_t *dctx,
	int opcode,
	void *mem_address
);

/**
 * @brief finds an instruction that references the given string
 * 
 * @param code_start address to start searching from
 * @param code_end address to stop searching at
 * @param str the target of the string reference (i.e. the target of the LEA instruction)
 * @return u8* the address of the first instruction that references the given string, or NULL if not found
 */
extern u8 *find_string_reference(
	u8 *code_start,
	u8 *code_end,
	const char *str
);

/**
 * @brief finds an instruction that references the given string
 * 
 * @param elf_info the parsed ELF context
 * @param encoded_string_id the string to search for, in encoded form
 * @param code_start address to start searching from
 * @param code_end address to stop searching at
 * @return u8* the address of the first instruction that references the given string, or NULL if not found
 */
extern u8 *elf_find_string_reference(
	elf_info_t *elf_info,
	EncodedStringId encoded_string_id,
	u8 *code_start,
	u8 *code_end
);

/**
 * @brief finds a reg2reg instruction
 *
 * a reg2reg instruction is an x64 instruction with one of the following characteristics:
 * - a primary opcode of 0x89 (MOV/JNS)
 * or, alternatively, passing the following filter:
 * - ((0x505050500000505uLL >> (((dctx->opcode) & 0xFF) + 0x7F)) & 1) != 0
 * NOTE: the opcode in 'dctx->opcode' is the actual opcode +0x80 
 * TODO: inspect x64 manual to find the exact filter
 *
 * the instruction must also satisfy the following conditions:
 * - NOT have REX.B and REX.R set (no extension bits)
 * - MODRM.mod must be 3 (register-direct addressing mode)
 * 
 * @param code_start address to start searching from
 * @param code_end address to stop searching at
 * @param dctx disassembler context to hold the state
 * @return BOOL TRUE if found, FALSE otherwise
 */
extern BOOL find_reg2reg_instruction(u8 *code_start, u8 *code_end, dasm_ctx_t *dctx);

/**
 * @brief locates the function prologue
 * 
 * @param code_start address to start searching from
 * @param code_end address to stop searching at
 * @param output pointer to receive the resulting prologue address, if found
 * @param find_mode prologue search mode/strategy
 * @return BOOL TRUE if found, FALSE otherwise
 */
extern BOOL find_function_prologue(u8 *code_start, u8 *code_end, u8 **output, FuncFindType find_mode);

/**
 * @brief locates the function boundaries.
 * 
 * @param code_start address to start searching from
 * @param func_start if provided, will be filled with the function's start address
 * @param func_end if provided, will be filled with the function's end address
 * @param search_base lowest search address, where search will be aborted
 * @param code_end address to stop searching at
 * @param find_mode 
 * @return BOOL 
 */
extern BOOL find_function(
	u8 *code_start,
	void **func_start,
	void **func_end,
	u8 *search_base,
	u8 *code_end,
	FuncFindType find_mode);

/**
 * @brief checks if given ELF file contains the range [vaddr, vaddr+size)
 * in a segment with the specified memory protection flags
 * 
 * @param elf_info elf context
 * @param vaddr starting memory address
 * @param size memory size
 * @param p_flags the expected segment protection flags (PF_*)
 * @return BOOL TRUE if found, FALSE otherwise
 */
extern BOOL elf_contains_vaddr(elf_info_t *elf_info, u64 vaddr, u64 size, u32 p_flags);

/**
 * @brief Parses the given in-memory ELF file into elf_info
 * 
 * @param ehdr pointer to the beginning of the ELF header
 * @param elf_info pointer to the structure that will hold the parsed information
 * @return BOOL TRUE if parsing completed successfully, FALSE otherwise
 */
extern BOOL elf_parse(Elf64_Ehdr *ehdr, elf_info_t *elf_info);

/**
 * @brief parses the main executable from the provided structure.
 * as part of the process, argv0 will be retrieved and checked 
 * to see if it's the expected one (/usr/sbin/sshd)
 * 
 * @param main_elf the main executable to parse
 * @return BOOL TRUE if successful, FALSE otherwise
 */
extern BOOL main_elf_parse(main_elf_t *main_elf);

/**
 * @brief parses the ELF rodata section, looking for strings and the instructions that reference them
 * 
 * @param elf_info the executable to find strings in
 * @param refs structure that will be populated with the results
 * @return BOOL 
 */
extern void elf_find_string_references(elf_info_t *elf_info, string_references_t *refs);

/**
 * @brief Looks up an ELF symbol from a parsed ELF
 * 
 * @param elf_info the parsed ELF context
 * @param encoded_string_id string ID of the symbol name
 * @param sym_version optional string representing the symbol version (e.g. "GLIBC_2.2.5")
 * @return Elf64_Sym* pointer to the ELF symbol, or NULL if not found
 */
extern Elf64_Sym *elf_symbol_get(elf_info_t *elf_info, EncodedStringId encoded_string_id, EncodedStringId sym_version);

/**
 * @brief Looks up an ELF symbol from a parsed ELF, and returns its memory address
 * 
 * @param elf_info the parsed ELF context
 * @param encoded_string_id string ID of the symbol name
 * @return void* the address of the symbol
 */
extern void *elf_symbol_get_addr(elf_info_t *elf_info, EncodedStringId encoded_string_id);

/**
 * @brief Obtains the address and size of the first executable segment in the given ELF file
 * 
 * @param elf_info the parsed ELF context, which will be updated with the address and size of the code segment
 * @param pSize variable that will be populated with the page-aligned segment size
 * @return void* the page-aligned starting address of the segment
 */
extern void *elf_get_code_segment(elf_info_t *elf_info, u64 *pSize);

/**
 * @brief Obtains the address and size of the last readonly segment in the given ELF file
 * this corresponds to the segment that typically contains .rodata
 * 
 * @param elf_info the parsed ELF context, which will be updated with the address and size of the rodata segment
 * @param pSize variable that will be populated with the page-aligned segment size
 * @return void* the page-aligned starting address of the segment
 */
extern void *elf_get_rodata_segment(elf_info_t *elf_info, u64 *pSize);

/**
 * @brief Obtains the address and size of the last read-write segment in the given ELF file
 * this is typically the segment that contains the following sections:
 * - .init_array .fini_array .data.rel.ro .dynamic .got
 * 
 * the parameter @p get_alignment controls if @p pSize should be populated with the segment size (when FALSE),
 * or with the segment alignment (when TRUE)
 *
 * @param elf_info the parsed ELF context, which will be updated with the address and size of the data segment
 * @param pSize variable that will be populated with either the page-aligned segment size, or the alignment size
 * @param get_alignment controls if alignment size should be returned instead of segment size
 * @return void* the page-aligned starting address of the segment
 */
extern void *elf_get_data_segment(elf_info_t *elf_info, u64 *pSize, BOOL get_alignment);

/**
 * @brief Searches the ELF relocations for a symbol having name @p encoded_string id
 * and relocation of type @p reloc_type
 * 
 * @param elf_info the parsed ELF context
 * @param relocs array of relocations to search in
 * @param num_relocs number of items in the array pointed by @p relocs
 * @param reloc_type type of relocation to consider (R_X86_64_*)
 * @param encoded_string_id symbol to look for (encoded)
 * @return void* the address of the symbol, or NULL if not found
 */
extern void *elf_get_reloc_symbol(
	elf_info_t *elf_info,
	Elf64_Rela *relocs,
	unsigned num_relocs,
	unsigned reloc_type,
	EncodedStringId encoded_string_id);

/**
 * @brief Gets the PLT symbol with name @p encoded_string_id from the parsed ELF file
 * 
 * @param elf_info the parsed ELF context
 * @param encoded_string_id symbol to look for (encoded)
 * @return void* the address of the symbol, or NULL if not found
 */
extern void *elf_get_plt_symbol(elf_info_t *elf_info, EncodedStringId encoded_string_id);

/**
 * @brief Gets the GOT symbol with name @p encoded_string_id from the parsed ELF file
 * 
 * @param elf_info the parsed ELF context
 * @param encoded_string_id symbol to look for (encoded)
 * @return void* the address of the symbol, or NULL if not found
 */
extern void *elf_get_got_symbol(elf_info_t *elf_info, EncodedStringId encoded_string_id);

/**
 * @brief Locates a string in the ELF .rodata section
 * 
 * @param elf_info the ELF context to use for the search
 * @param stringId_inOut mandatory pointer to an encoded string ID.
 * - if the referenced string ID is 0, the first matching string (in the string table) will stop the search,
 * and the matching string ID will be written to the pointer.
 * - if the referenced string ID is not 0, the search will look for that specific string ID,
 * and the value will not be updated.
 * @param rodata_start_ptr location in the rodata section to start the search from
 * @return char* pointer to the string, or NULL if it couldn't be found
 */
extern char *elf_find_string(
	elf_info_t *elf_info,
	u32 *stringId_inOut,
	void *rodata_start_ptr);

/**
 * @brief gets the fake LZMA allocator, used for imports resolution
 * the "opaque" field of the structure holds a pointer to @see elf_info_t
 * 
 * @return lzma_allocator* 
 */
extern lzma_allocator *get_lzma_allocator();

extern BOOL secret_data_append_from_instruction(dasm_ctx_t *dctx, secret_data_shift_cursor *cursor);

/**
 * @brief Pushes secret data by validating the given code block
 * 
 * @param code_start pointer to the beginning of code/function to analyze
 * @param code_end pointer to the end of code/function to analyze
 * @param shift_cursor shift index
 * @param shift_count how many '1' bits to shift
 * @param start_from_call TRUE if analysis should begin from the first CALL instruction
 * FALSE to start from the first instruction
 * @return BOOL TRUE if all requested shifts were all executed.
 * FALSE if some shift wasn't executed due to code validation failure.
 */
extern BOOL secret_data_append_from_code(
	void *code_start,
	void *code_end,
	secret_data_shift_cursor shift_cursor,
	unsigned shift_count, BOOL start_from_call);

/**
 * @brief Calls @ref secret_data_append_singleton, if @p flags are non-zero
 *
 * @param shift_cursor the initial shift index
 * @param operation_index identification for this shift operation
 * @param shift_count how many '1' bits to shift
 * @param flags must be non-zero in order for the operation to be executed
 * @param code pointer to code that will be checked by the function, to "authorize" the data load
 * @return BOOL TRUE if validation was successful and data was added, FALSE otherwise
 */
extern BOOL secret_data_append_if_flags(
	secret_data_shift_cursor shift_cursor,
	unsigned operation_index,
	unsigned shift_count,
	int flags, u8 *code);

/**
 * @brief Shifts data in the secret data store, after validation of @p code.
 * this function is intended to be invoked only once for each @p operation_index value.
 * @p operation_index will be used as an index into a global array of flags,
 * so that multiple calls with the same value will be a NO-OP.
 * 
 * the @p code will be verified to check if the shift operation should be allowed or not.
 * the algorithm will:
 * - locate the beginning of the function, by scanning for the `endbr64` instruction
 *    and making sure that the code lies between a pre-defined code range (set in @ref backdoor_setup from @ref elf_get_code_segment)
 * - search for @p shift_count number of "reg2reg" instructions (explained below)
 * - for each instruction, shift a '1' in the data register, and increment the shift cursor to the next bit index
 * the code only considers reg2reg instruction. other instructions are skipped.
 * the function will return TRUE if the number of shifts executed == number of wanted shifts
 * (that is, if there are as many compatible reg2reg instructions as the number of requested shifts)
 * NOTE: MOV instructions are counted, but don't cause any shift (they are skipped).
 *
 * a reg2reg instruction is an x64 instruction with one of the following characteristics:
 * - primary opcode of 0x89 (MOV) or 0x3B (CMP)
 * or, alternatively, an opcode that passes the following validation
 *  opcode_check = opcode - 0x83;
 *  if ( opcode_check > 0x2E || ((0x410100000101 >> opcode_value) & 1) == 0 )
 *
 * additionally, checks outlined in @ref find_reg2reg_instruction must also pass
 * NOTE: the opcode in 'opcode' is the actual opcode +0x80 
 * TODO: inspect x64 manual to find the exact filter
 *
 * if @p call_site is supplied, a preliminary check will be conducted to see if the caller function
 * contains a CALL-relative instruction.
 * several functions have a CALL in the prologue which serves a dual purpose:
 * - push more data in the secret data store
 * - check if the call is authorized (the code is in the authorized range, and starts with a CALL-relative instruction)
 *
 *
 * @param call_site if supplied, it will be checked if it contains a valid CALL-relative instruction
 * @param code pointer to code that will be checked by the function, to "authorize" the data load
 * @param shift_cursor the initial shift index
 * @param shift_count number of '1' bits to shift, represented by the number of"reg2reg" instructions expected in the function pointed to by @p code
 * @param operation_index index/id of shit shift operation
 * @return BOOL TRUE if all requested shifts were all executed.
 * FALSE if some shift wasn't executed due to code validation failure.
 */
extern BOOL secret_data_append_singleton(
	u8 *call_site, u8 *code,
	secret_data_shift_cursor shift_cursor,
	unsigned shift_count, unsigned operation_index);

/**
 * @brief Shifts data in the secret data store, after validation of the call site,
 * i.e. the caller of this function
 * for more details, see @ref secret_data_append_singleton
 * 
 * @param shift_cursor the initial shift index
 * @param shift_count number of '1' bits to shift
 * @param operation_index index/id of shit shift operation
 * @param bypass forces the result to be TRUE, evne if validation failed
 * @return BOOL TRUE if validation was successful and data was added, FALSE otherwise
 */
extern BOOL secret_data_append_from_call_site(
	secret_data_shift_cursor shift_cursor,
	unsigned shift_count, unsigned operation_index,
	BOOL bypass
);

/**
 * @brief the backdoor main method
 * 
 * @param params parameters
 * @return BOOL unused
 */
extern BOOL backdoor_setup(backdoor_setup_params_t *params);

extern void backdoor_init(elf_entry_ctx_t *ctx, u64 *caller_frame);
extern BOOL backdoor_init_stage2(elf_entry_ctx_t *ctx);

/**
 * @brief parses the libc ELF from the supplied link map, and resolves its imports
 * 
 * @param libc the loaded libc's link map (obtained by traversing r_debug->r_map)
 * @param libc_info pointer to an ELF context that will be populated with the parsed ELF information
 * @param imports pointer to libc imports that will be populated with resolved libc function pointers
 * @return BOOL TRUE if successful, FALSE otherwise
 */
extern BOOL resolve_libc_imports(
	struct link_map *libc,
	elf_info_t *libc_info,
	libc_imports_t *imports
);

/**
 * @brief scans loaded libraries to identify interesting libraries
 * 
 * @param data input data for the function (will be duplicated, internally)
 * @return BOOL TRUE if successful, FALSE otherwise
 */
extern BOOL process_shared_libraries(backdoor_shared_libraries_data_t *data);

/**
 * @brief scans loaded libraries to identify interesting libraries and populate related data
 * 
 * @param r_map the linked list of loaded libraries obtained from `r_debug`
 * @param data pointer to data that will be populated by the function
 * @return BOOL TRUE if successful, FALSE otherwise
 */
extern BOOL process_shared_libraries_map(struct link_map *r_map, backdoor_shared_libraries_data_t *data);

/**
 * @brief decrypts a buffer with chacha20
 * 
 * @param in the input buffer to decrypt
 * @param inl the length of the input buffer
 * @param key the 256bit chacha key
 * @param iv the 128bit chacha iv
 * @param out the output buffer
 * @param funcs OpenSSL imported functions
 * @return BOOL TRUE if successful, FALSE otherwise
 */
extern BOOL chacha_decrypt(
	u8 *in, int inl,
	u8 *key, u8 *iv,
	u8 *out, imported_funcs_t *funcs
);

/**
 * @brief obtains a decrypted copy of the secret data
 * 
 * @param output output buffer that will receive the decrypted data
 * @param ctx the global context (for secret data and function imports)
 * @return BOOL TRUE if successful, FALSE otherwise
 */
extern BOOL secret_data_get_decrypted(u8 *output, global_context_t *ctx);

/**
 * @brief verify if a memory range is mapped
 * 
 * @param addr the start address
 * @param length the length of the range to check
 * @param ctx a structure with a libc_import_t field at offset 0x10
 * @return BOOL TRUE if the whole range is mapped, FALSE otherwise
 */
extern BOOL is_range_mapped(u8* addr, u8 length, global_context_t* ctx);

/**
 * @brief Get the @see EncodedStringId for the given string
 * the string will be consumed until one of the following condition is reached (whichever happens first):
 * - 44 chars have been consumed (maximum string length)
 * - @p string_end is supplied and has been reached
 * - the string table has been exhausted
 * 
 * @param string_begin the string to get the ID for (max 44 chars)
 * @param string_end optional string end pointer
 * @return EncodedStringId the string ID matching the input string, or 0 if not found
 */
extern EncodedStringId get_string_id(const char *string_begin, const char *string_end);

/**
 * @brief the backdoor entrypoint function, called by the IFUNC resolver
 * 
 * @param cpuid_request 
 * @return BOOL 
 */
extern BOOL _get_cpuid(int cpuid_request, void*, void*, void*, void*, void*);

/**
 * @brief Initializes the structure with hooks-related data
 * 
 * @param funcs 
 * @return int 
 */
extern int init_hook_functions(backdoor_hooks_ctx_t *funcs);

/**
 * @brief recomputes the GOT address
 * 
 * @param entry_ctx 
 */
extern void update_got_address(elf_entry_ctx_t *entry_ctx);

/**
 * @brief the backdoored symbind64 installed in GLRO(dl_audit)
 * 
 * @param sym 
 * @param ndx 
 * @param refcook 
 * @param defcook 
 * @param flags 
 * @param symname 
 * @return uintptr_t 
 */
extern uintptr_t backdoor_symbind64(
	Elf64_Sym *sym,
	unsigned int ndx,
	uptr *refcook, uptr *defcook,
	unsigned int flags,
	const char *symname);

extern u32 resolver_call_count;
extern global_context_t *global_ctx;
extern lzma_allocator *fake_lzma_allocator;
extern backdoor_hooks_data_t **hooks_data_addr;

#include "util.h"
#endif