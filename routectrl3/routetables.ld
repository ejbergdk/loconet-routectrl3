/* Linker script augmentation */

SECTIONS
{
  routetables :
  {
    __progmemx_start = .;
    *(.progmemx.text*)
    *(.progmemx.data*)
    __progmemx_end = .;
    PROVIDE (__loconet_fbocctable_start = .) ;
    *(SORT_BY_NAME(loconet.fbocctable*))
    PROVIDE (__loconet_fbocctable_end = .) ;
    KEEP(*(loconet.fbocctable*))
    PROVIDE (__loconet_fbfreetable_start = .) ;
    *(SORT_BY_NAME(loconet.fbfreetable*))
    PROVIDE (__loconet_fbfreetable_end = .) ;
    KEEP(*(loconet.fbfreetable*))
    PROVIDE (__loconet_fbrangeocctable_start = .) ;
    *(loconet.fbrangeocctable)
    PROVIDE (__loconet_fbrangeocctable_end = .) ;
    KEEP(*(loconet.fbrangeocctable))
    PROVIDE (__loconet_fbrangefreetable_start = .) ;
    *(loconet.fbrangefreetable)
    PROVIDE (__loconet_fbrangefreetable_end = .) ;
    KEEP(*(loconet.fbrangefreetable))
    PROVIDE (__loconet_swreqtable_start = .) ;
    *(SORT_BY_NAME(loconet.swreqtable*))
    PROVIDE (__loconet_swreqtable_end = .) ;
    KEEP(*(loconet.swreqtable*))
    PROVIDE (__loconet_swreqrangetable_start = .) ;
    *(loconet.swreqrangetable)
    PROVIDE (__loconet_swreqrangetable_end = .) ;
    KEEP(*(loconet.swreqrangetable))
    PROVIDE (__loconet_routetable_start = .) ;
    *(loconet.routetable)
    PROVIDE (__loconet_routetable_end = .) ;
    KEEP(*(loconet.routetable))
    . = ALIGN(2);
  }
}

INSERT AFTER .text
