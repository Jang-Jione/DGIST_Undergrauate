module assign_1_C (
  output reg Q,
  input D, Clk, PreN, ClrN);
  
  always @ (posedge Clk, negedge PreN, negedge ClrN) // posedge: positive edge, negedge: negative edge
    if (!ClrN) Q <= 1'b0;
  else if (!PreN) Q <= 1'b1;
  	else Q <= D;
 
endmodule