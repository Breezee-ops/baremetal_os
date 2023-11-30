volatile int arr[6];
volatile int* top, bottom;
volatile int call;

void create_queue();
void enqueue(int function);
int dequeue();