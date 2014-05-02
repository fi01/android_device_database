#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "device_database.h"
#include "device_address.c"
 
#define SQL_CREATE_SUPPORTED_DEVICES \
  "CREATE TABLE supported_devices(" \
    "device_id INTEGER PRIMARY KEY ASC, " \
    "device TEXT NOT NULL, " \
    "build_id TEXT NOT NULL, " \
    "check_property_name TEXT, " \
    "check_property_value TEXT, " \
    "UNIQUE (device, build_id, check_property_name, check_property_value));" \

#define SQL_CREATE_DEVICES_ADDRESS \
  "CREATE TABLE device_address(" \
    "device_id INTEGER NOT NULL, " \
    "name TEXT NOT NULL, " \
    "value TEXT NOT NULL, " \
    "PRIMARY KEY(device_id, name));" \

#define SQL_INSERT_SUPPORTED_DEVICES \
  "INSERT INTO supported_devices(" \
    "device_id, " \
    "device, " \
    "build_id, " \
    "check_property_name, " \
    "check_property_value) " \
  "VALUES(" \
    "%d, " \
    "'%s', " \
    "'%s', " \
    "NULL, " \
    "NULL);"

#define SQL_INSERT_SUPPORTED_DEVICES_WITH_CHECK_PROPERTY \
  "INSERT INTO supported_devices(" \
    "device_id, " \
    "device, " \
    "build_id, " \
    "check_property_name, " \
    "check_property_value) " \
  "VALUES(" \
    "%d, " \
    "'%s', " \
    "'%s', " \
    "'%s', " \
    "'%s');"

#define SQL_INSERT_DEVICES_ADDRESS \
  "INSERT INTO device_address(" \
    "device_id, " \
    "name, " \
    "value) " \
  "VALUES(" \
    "%d, " \
    "'%s', " \
    "'0x%08lx');"

#define WRITE_SQL_INSERT_DEVICE_ADDRESS(device,key) \
    if (device->key## _address) { \
      printf("  " SQL_INSERT_DEVICES_ADDRESS "\n", \
             device->device_id, \
	     #key, \
	     device->key## _address); \
    }

static int cmp(const void *a, const void *b)
{
  const supported_device *x = a; 
  const supported_device *y = b; 

  return x->device_id - y->device_id;
}

int main(void)
{
  int i;

  qsort(supported_devices, n_supported_devices, sizeof (supported_devices[0]), cmp);

  printf("BEGIN TRANSACTION;\n");
  printf("DROP TABLE IF EXISTS supported_devices;\n");
  printf("DROP TABLE IF EXISTS device_address;\n");
  printf("%s\n\n", SQL_CREATE_SUPPORTED_DEVICES);
  printf("%s\n\n", SQL_CREATE_DEVICES_ADDRESS);

  for (i = 0; i < n_supported_devices; i++) {
    supported_device *device = &supported_devices[i];

    if (!device->check_property_name) {
      printf(SQL_INSERT_SUPPORTED_DEVICES "\n",
             device->device_id,
             device->device,
             device->build_id);
    }
    else {
      printf(SQL_INSERT_SUPPORTED_DEVICES_WITH_CHECK_PROPERTY "\n",
             device->device_id,
             device->device,
             device->build_id,
             device->check_property_name,
             device->check_property_value);
    }

    WRITE_SQL_INSERT_DEVICE_ADDRESS(device, kernel_physical_offset)
    WRITE_SQL_INSERT_DEVICE_ADDRESS(device, prepare_kernel_cred)
    WRITE_SQL_INSERT_DEVICE_ADDRESS(device, commit_creds)
    WRITE_SQL_INSERT_DEVICE_ADDRESS(device, remap_pfn_range)
    WRITE_SQL_INSERT_DEVICE_ADDRESS(device, vmalloc_exec)
    WRITE_SQL_INSERT_DEVICE_ADDRESS(device, ptmx_fops)

    WRITE_SQL_INSERT_DEVICE_ADDRESS(device, security_remap_pfn_range)
    WRITE_SQL_INSERT_DEVICE_ADDRESS(device, remap_pfn_range_end_op)

    printf("\n");
  }
  printf("COMMIT;\n");

  return 0;
}
