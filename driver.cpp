#include <ntifs.h>
#include <ntddk.h>     
#include <ntstrsafe.h>

#define code_eac CTL_CODE(FILE_DEVICE_UNKNOWN, 0x900, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)


extern POBJECT_TYPE IoDriverObjectType;
NTSTATUS GetDriverObjectByName(PCWSTR DriverName, PDRIVER_OBJECT* DriverObject)
{
	UNICODE_STRING usDriverName;
	NTSTATUS status;
	PDRIVER_OBJECT tempDriverObject = NULL;
	OBJECT_ATTRIBUTES objectAttributes;

	UNICODE_STRING functionName;
	RtlInitUnicodeString(&functionName, L"ObReferenceObjectByName");
	NTSTATUS(*ObReferenceObjectByNameFunc)(
		PUNICODE_STRING,
		ULONG,
		PACCESS_STATE,
		ACCESS_MASK,
		POBJECT_TYPE,
		KPROCESSOR_MODE,
		PVOID,
		PVOID*
		) = (NTSTATUS(*)(PUNICODE_STRING, ULONG, PACCESS_STATE, ACCESS_MASK, POBJECT_TYPE, KPROCESSOR_MODE, PVOID, PVOID*))MmGetSystemRoutineAddress(&functionName);

	if (ObReferenceObjectByNameFunc == NULL) {
		DbgPrint("Failed to resolve ObReferenceObjectByName\n");
		return STATUS_UNSUCCESSFUL;
	}

	RtlInitUnicodeString(&functionName, L"IoDriverObjectType");
	POBJECT_TYPE IoDriverObjectType = (POBJECT_TYPE)MmGetSystemRoutineAddress(&functionName);

	if (IoDriverObjectType == NULL) {
		DbgPrintEx(0,0,"Failed to resolve IoDriverObjectType\n");
		return STATUS_UNSUCCESSFUL;
	}

	RtlInitUnicodeString(&usDriverName, DriverName);

	InitializeObjectAttributes(&objectAttributes, &usDriverName, OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE, NULL, NULL);

	status = ObReferenceObjectByNameFunc(
		&usDriverName,                
		OBJ_CASE_INSENSITIVE,
		NULL,                         
		0,                            
		IoDriverObjectType,           
		KernelMode,                   
		NULL,                         
		(PVOID*)&tempDriverObject    
	);

	if (NT_SUCCESS(status)) {
		*DriverObject = tempDriverObject;
	}
	else {
		*DriverObject = NULL;
	}

	return status;
}


NTSTATUS hook(PDRIVER_OBJECT targetObject)
{
	targetObject->MajorFunction[IRP_MJ_CREATE] = NULL;
	targetObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = NULL;
	targetObject->MajorFunction[IRP_MJ_CLOSE] = NULL;
	for (int i = 0; i <= IRP_MJ_MAXIMUM_FUNCTION; i++) {
		targetObject->MajorFunction[i] = NULL;
	}
	DbgPrintEx(0, 0, "EAC nullified.");
	return STATUS_SUCCESS;
}

NTSTATUS io_controller(PDEVICE_OBJECT device_obj, PIRP irp) {
	UNREFERENCED_PARAMETER(device_obj);

	NTSTATUS status = { };
	ULONG bytes = { };
	PIO_STACK_LOCATION stack = IoGetCurrentIrpStackLocation(irp);

	ULONG code = stack->Parameters.DeviceIoControl.IoControlCode;
	ULONG size = stack->Parameters.DeviceIoControl.InputBufferLength;

	if (code == code_eac) {
		NTSTATUS status;
		PDRIVER_OBJECT pDriverObject = NULL;

		status = GetDriverObjectByName(L"\\Driver\\EasyAntiCheat", &pDriverObject);

		if (NT_SUCCESS(status)) {
			DbgPrintEx(0, 0, "EAC object obtained: 0x%p\n", pDriverObject);
			hook(pDriverObject);
			ObDereferenceObject(pDriverObject);
		}
		else {
			DbgPrintEx(0,0,"Failed to get EAC object: 0x%x\n", status);
		}
	}


	irp->IoStatus.Status = status;
	irp->IoStatus.Information = bytes;
	IoCompleteRequest(irp, IO_NO_INCREMENT);

	return status;
}



NTSTATUS DriverEntry(PDRIVER_OBJECT driver_obj, PUNICODE_STRING reg_path)
{
	// implement your own entry here

	return status;
}
