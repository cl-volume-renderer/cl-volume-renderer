__kernel void hello_world(__global int* in, int size)
{
  if(get_global_id(0) < size){
    in[get_global_id(0)] = in[get_global_id(0)] + 1;
  }
}
