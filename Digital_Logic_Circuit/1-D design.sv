module assign_1_C (
  output reg Q,
  input D, Clk, PreN, ClrN
);
  
  always @(posedge Clk or negedge PreN or negedge ClrN) begin
    if (!ClrN) Q <= 1'b0;
    else if (!PreN) Q <= 1'b1;
    else Q <= D;
  end
endmodule

module assign_1_B_FullAdder (
  input A, B, C_, // A, B, 아래서 올라온 carry
  output S, C // sum, carry
);
  wire w1, w2, w3;
    
  xor G1 (w1, A, B);
  xor G2 (S, w1, C_);

  and G3 (w2, A, B);
  and G4 (w3, w1, C_);
  or G5 (C, w2, w3);
endmodule

module SerialAdder (
  output reg [3:0] sum_out,
  output wire carry_out,
  input wire clk,
  input wire rst,
  input wire [3:0] in_a,
  input wire [3:0] in_b
);

  reg [3:0] d_out;
  wire carry_in = 1'b0;
  wire carry0 = 1'b0;
  wire carry1 = 1'b0;
  wire carry2 = 1'b0;
  wire carry3 = 1'b0;
  wire carry4 = 1'b0;
  wire carry5 = 1'b0;
  
  // Full adder instances
  assign_1_B_FullAdder U1 (
    .A(in_a[0]),
    .B(in_b[0]),
    .C_(carry_in),
    .S(sum_out[0]),
    .C(carry0)
  );
  
  assign_1_C U11 (
    .Q(carry1),
    .D(carry0),
    .Clk(clk),
    .PreN(rst),
    .ClrN(~rst)
  );
  
  assign_1_B_FullAdder U2 (
    .A(in_a[1]),
    .B(in_b[1]),
    .C_(carry1),
    .S(sum_out[1]),
    .C(carry2)
  );
  
  assign_1_C U12 (
    .Q(carry3),
    .D(carry2),
    .Clk(clk),
    .PreN(rst),
    .ClrN(~rst)
  );
  
  assign_1_B_FullAdder U3 (
    .A(in_a[2]),
    .B(in_b[2]),
    .C_(carry3),
    .S(sum_out[2]),
    .C(carry4)
  );
  
  assign_1_C U13 (
    .Q(carry5),
    .D(carry4),
    .Clk(clk),
    .PreN(rst),
    .ClrN(~rst)
  );
  
  assign_1_B_FullAdder U4 (
    .A(in_a[3]),
    .B(in_b[3]),
    .C_(carry5),
    .S(sum_out[3]),
    .C(carry_out)
  );
endmodule