#include <linux/module.h>
#include <linux/export-internal.h>
#include <linux/compiler.h>

MODULE_INFO(name, KBUILD_MODNAME);

__visible struct module __this_module
__section(".gnu.linkonce.this_module") = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};



static const struct modversion_info ____versions[]
__used __section("__versions") = {
	{ 0xb1ad28e0, "__gnu_mcount_nc" },
	{ 0xefd6cf06, "__aeabi_unwind_cpp_pr0" },
	{ 0x92997ed8, "_printk" },
	{ 0xc1514a3b, "free_irq" },
	{ 0x56044fa2, "module_put" },
	{ 0x37a0cba, "kfree" },
	{ 0xfd0f592, "mutex_trylock" },
	{ 0x9618ede0, "mutex_unlock" },
	{ 0xbb72d4fe, "__put_user_1" },
	{ 0x24990333, "kmalloc_caches" },
	{ 0x3881d502, "__kmalloc_cache_noprof" },
	{ 0x5f754e5a, "memset" },
	{ 0xcc4bc881, "gpio_to_desc" },
	{ 0xaa1042bd, "gpiod_to_irq" },
	{ 0x92d5838e, "request_threaded_irq" },
	{ 0x80b0008f, "try_module_get" },
	{ 0xf0fdf6cb, "__stack_chk_fail" },
	{ 0x8f678b07, "__stack_chk_guard" },
	{ 0x28118cb6, "__get_user_1" },
	{ 0x326eca89, "gpiod_set_raw_value" },
	{ 0x6bc3fbc0, "__unregister_chrdev" },
	{ 0xfe990052, "gpio_free" },
	{ 0xde4bf88b, "__mutex_init" },
	{ 0x3c3ff9fd, "sprintf" },
	{ 0x47229b5c, "gpio_request" },
	{ 0xffbaf89d, "gpiod_direction_output_raw" },
	{ 0xf72c984d, "gpiod_direction_input" },
	{ 0x6e064dd8, "__register_chrdev" },
	{ 0x1b947acd, "param_ops_charp" },
	{ 0xdcffdfcd, "param_ops_int" },
	{ 0x8a3dbe73, "module_layout" },
};

MODULE_INFO(depends, "");


MODULE_INFO(srcversion, "F69127A0BE1C86A30C83A0E");
