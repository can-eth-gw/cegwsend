---
layout: index
title: man page
subtitle: CAN-ETH-GW(1) CAN - Ethernet Gateway Send Utility User Manual |
master-preview
---

NAME
====

cegwsend - CAN - Ethernet Gateway Send Utility

SYNOPSIS
========

**cegwsend** [ **-s** *SRC-MAC* | **--src**=*SRC-MAC* ] [ **-d**
*DST-MAC* | **-dst**=*DST-MAC* ] [ **-t** *ETHERTYPE* |
**--type**=*ETHERTYPE* ] { **-r** *RAW* | **--raw**=*RAW* } { **-i**
*IFACE* | **--interface**=*IFACE* }

*ETHERTYPE* := { **ipv4** | **ipv6** | **can** | **canfd** | **none** |
**[0x]1234** }

DESCRIPTION
===========

Send Utility for testing non ip-based ethernet interfaces. You can send
with this utility an ethernet package with raw data or with an
CAN/CAN-FD Frame as payload.

OPTIONS
=======

**-s**, **--src**=*SRC-MAC* 
> The MAC address, which should be written
down in the SOURCE field of the ethernet header. In the current version
this option is NOT USED.

**-d**, **--dst**=*DST-MAC* 
> The MAC address, which should be written
down in the DESTINATION field of the ethernet header.

**-t**, **--type**=*ETHERTYPE* 
> Set the EtherType of the Ethernet
frame. '**ipv4**', '**ipv6**', '**can**' and '**canfd**' can be
translated to their hexadcimal code. All other types must be written
like '**0x1234**' or '**1234**. Please note that the last example is a
hexadecimal number, too. '**none**' can be used for experimental
purposes.

**-r**, **--raw**=*RAW* 
> The raw data that should be sent. Please
notice that the raw data must be typed in the console as a hexadecimal
number like this "01 23 45 67 89 ab cd ef", for example. You can use
capital letters for A, B, C, D, E and F, of course.

**-i**, **--interface**=*IFACE* 
> The name of the Ethernet Interface,
that is going to receive the message

EXIT STATUS
===========

<table>
<thead>
<tr class="header">
<th align="center">CODE</th>
<th align="left">Description</th>
</tr>
</thead>
<tbody>
<tr class="odd">
<td align="center">0</td>
<td align="left">success</td>
</tr>
<tr class="even">
<td align="center">!=0</td>
<td align="left">failure</td>
</tr>
</tbody>
</table>

BUGS
====

This is an early preview version. They are maybe some bugs. Please
report bugs at [GitHub can\_eth\_gw
Issues](https://github.com/can-eth-gw/can_eth_gw/issues)

COPYRIGHT
=========

© Copyright 2013 Fabian Raab, Stefan Smarzly

This file is part of CAN-Eth-GW.

CAN-Eth-GW is free software: you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the
Free Software Foundation, either version 3 of the License, or (at your
option) any later version.

CAN-Eth-GW is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
Public License for more details. You should have received a copy of the
GNU General Public License along with CAN-Eth-GW. If not, see
<http://www.gnu.org/licenses/>.

SEE ALSO
========

**Homepage:** <http://can-eth-gw.github.io>

AUTHORS
=======
Jakob Pfeiffer <jakob.pfeiffer@in.tum.de>.
