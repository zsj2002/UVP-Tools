#ifndef AGP_H
#define AGP_H 1

#include <asm/cacheflush.h>
#include <asm/system.h>

/*
 * Functions to keep the agpgart mappings coherent.
 * The GART gives the CPU a physical alias of memory. The alias is
 * mapped uncacheable. Make sure there are no conflicting mappings
 * with different cachability attributes for the same page.
 */

#define map_page_into_agp(page) ( \
	xen_create_contiguous_region((unsigned long)page_address(page), 0, 32) \
	?: change_page_attr(page, 1, PAGE_KERNEL_NOCACHE))
#define unmap_page_from_agp(page) ( \
	xen_destroy_contiguous_region((unsigned long)page_address(page), 0), \
	/* only a fallback: xen_destroy_contiguous_region uses PAGE_KERNEL */ \
	change_page_attr(page, 1, PAGE_KERNEL))
#define flush_agp_mappings() global_flush_tlb()

/* Could use CLFLUSH here if the cpu supports it. But then it would
   need to be called for each cacheline of the whole page so it may not be
   worth it. Would need a page for it. */
#define flush_agp_cache() wbinvd()

/* Convert a physical address to an address suitable for the GART. */
#define phys_to_gart(x) phys_to_machine(x)
#define gart_to_phys(x) machine_to_phys(x)

/* GATT allocation. Returns/accepts GATT kernel virtual address. */
#define alloc_gatt_pages(order)	({                                          \
	char *_t; dma_addr_t _d;                                            \
	_t = dma_alloc_coherent(NULL,PAGE_SIZE<<(order),&_d,GFP_KERNEL);    \
	_t; })
#define free_gatt_pages(table, order)	\
	dma_free_coherent(NULL,PAGE_SIZE<<(order),(table),virt_to_bus(table))

#endif
