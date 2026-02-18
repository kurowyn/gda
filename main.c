#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>

#define ARRAYINT(...) ArrayInt_Init(sizeof ((int []){__VA_ARGS__}) / sizeof (int), \
									__VA_ARGS__)

enum {GDA_REMOVE_INDEX, GDA_REMOVE_ELEMENT, GDA_REMOVE_UNKNOWN, GDA_NULL_POINTER,
	  GDA_ELEMENT_NOT_FOUND, GDA_SUCCESS, GDA_FAILURE, GDA_OUT_OF_BOUNDS};

typedef struct {
	int *start;	
	size_t length;
} ArrayInt;

void
Gda_Handle_Null_Pointer(void *ptr){
	if (ptr) {
	//	fprintf(stdout, "[INFO] Pointer %p is clean.\n", ptr);
	} else {
		fprintf(stdout, "[INFO] Null pointer. Crashing (shrug face).\n");
		exit(GDA_NULL_POINTER);
	}
}

void *
Gda_Malloc(size_t size){
	void *ptr = malloc(size);
	Gda_Handle_Null_Pointer(ptr);
	return ptr;
}

void *
Gda_Calloc(size_t size, size_t count){
	void *ptr = calloc(size, count);
	Gda_Handle_Null_Pointer(ptr);
	return ptr;
}

void *
Gda_Realloc(void *ptr, size_t size){
	void *resized_ptr = realloc(ptr, size);
	Gda_Handle_Null_Pointer(ptr);
	return resized_ptr;
}

void
Gda_Free(void *ptr){
	free(ptr);
}

ArrayInt
ArrayInt_Init(int length, ...){
	va_list ap;
	va_start(ap, length);

	ArrayInt array = (ArrayInt){.start = Gda_Calloc(sizeof (int), length),
							    .length = length};

	for (int i = 0; i < length; ++i) {
		array.start[i] = va_arg(ap, int);
	}

	va_end(ap);
	return array;
}

int
ArrayInt_Index(ArrayInt array, int element){
	Gda_Handle_Null_Pointer(array.start);

	for (int i = 0; i < array.length; ++i) {
		if (array.start[i] == element) {
			return i;
		}
	}
	return GDA_ELEMENT_NOT_FOUND;
}

int
ArrayInt_Append(ArrayInt *array, int element){
	Gda_Handle_Null_Pointer(array);
	Gda_Handle_Null_Pointer(array->start);

	++array->length;
	array->start = Gda_Realloc(array->start, array->length * sizeof (int));
	array->start[array->length - 1] = element;
	return GDA_SUCCESS;
}

int
ArrayInt_Remove(ArrayInt *array, int payload, int type){
	Gda_Handle_Null_Pointer(array);
	Gda_Handle_Null_Pointer(array->start);

	int element, element_index;
	switch (type) {
		case GDA_REMOVE_INDEX:
			if (0 <= payload && payload < array->length) {
				element_index = payload;
				element = array->start[payload];
			} else {
				return GDA_OUT_OF_BOUNDS;	
			}
			break;
		case GDA_REMOVE_ELEMENT:
			element = payload;
			element_index = ArrayInt_Index(*array, payload);
			break;
		default:
			fprintf(stdout, "[INFO] Wrong type paramater used. No changes to array were made.\n");
			return GDA_REMOVE_UNKNOWN;
	}

	if (0 <= element_index && element_index < array->length) {
		memmove(array->start + element_index,
				array->start + element_index + 1,
				(array->length - 1) * sizeof (int));
		--array->length;
		array->start = Gda_Realloc(array->start, array->length * sizeof (int));
		return element;
	}

	fprintf(stdout, "[INFO] Element not found.\n");
	return GDA_ELEMENT_NOT_FOUND;
}

int
ArrayInt_Remove_Index(ArrayInt *array, int index){
	int element = ArrayInt_Remove(array, index, GDA_REMOVE_INDEX);
	return element;
}

int
ArrayInt_Remove_Element(ArrayInt *array, int element){
	ArrayInt_Remove(array, element, GDA_REMOVE_ELEMENT);
	return element;
}

int
ArrayInt_Pop(ArrayInt *array){
	int element = ArrayInt_Remove(array, array->length - 1, GDA_REMOVE_INDEX);	
	return element;
}

int
ArrayInt_Show(ArrayInt array){
	Gda_Handle_Null_Pointer(array.start);
	putchar('{');
	for (int i = 0; i < array.length; ++i) {
		printf("%d%s", array.start[i], i == array.length - 1 ? "" : ", ");
	}
	putchar('}');
	putchar('\n');
	return GDA_SUCCESS;
}

int
ArrayInt_Clear(ArrayInt *array){
	Gda_Handle_Null_Pointer(array);
	Gda_Handle_Null_Pointer(array->start);
	Gda_Free(array->start);
	array->length = 0;
	array->start = NULL;
	return GDA_SUCCESS;
}

int
ArrayInt_Count(ArrayInt array, int element){
	Gda_Handle_Null_Pointer(array.start);

	int count = 0;
	for (int i = 0; i < array.length; ++i) {
		count += array.start[i] == element;
	}
	return count;
}

int
ArrayInt_Extend(ArrayInt *array_1, ArrayInt array_2){ 
	Gda_Handle_Null_Pointer(array_1);
	Gda_Handle_Null_Pointer(array_1->start);
	Gda_Handle_Null_Pointer(array_2.start);

	for (int i = 0; i < array_2.length; ++i) {
		ArrayInt_Append(array_1, array_2.start[i]);
	}

	return GDA_SUCCESS;
}

int
ArrayInt_Insert(ArrayInt *array, int new, int element){
	Gda_Handle_Null_Pointer(array);
	Gda_Handle_Null_Pointer(array->start);

	int element_index = ArrayInt_Index(*array, element);	
	if (element_index != -1) {
		ArrayInt_Append(array, 0);
		memmove(array->start + element_index + 1, 
				array->start + element_index,
				(array->length - 1) * sizeof (int));
		array->start[element_index] = new;
		return GDA_SUCCESS;
	}
}

int
ArrayInt_Reverse(ArrayInt *array){
	if (array && array->start) {
		int head = 0, tail = array->length - 1;
		while (head < tail) {
			int temp = array->start[head];
			array->start[head] = array->start[tail];
			array->start[tail] = temp;
			++head;
			--tail;
		}
		return GDA_SUCCESS;
	}
	return GDA_NULL_POINTER;
}

int main(int argc, char **argv){
	ArrayInt array = ARRAYINT(1, 2, 3, 4, 5, 6);
	ArrayInt_Show(array);
	ArrayInt_Show(array);
	ArrayInt_Clear(&array);
	return 0;
}
