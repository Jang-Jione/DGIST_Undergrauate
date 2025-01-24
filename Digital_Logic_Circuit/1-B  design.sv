// ======== full adder ========
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

// ======== 4b parallel adder ========

module assign_1_B_4bParallelAdder (
  input [3:0] A, B,
  output [3:0] Sum, Cout
);
  wire [3:0] w_sum;
  wire w_carry1, w_carry2, w_carry3;

  // 모듈 사용은 주로 U 사용하는 듯!!
  assign_1_B_FullAdder U0 (.A(A[0]), .B(B[0]), .C_(1'b0), .S(w_sum[0]), .C(w_carry1));
  assign_1_B_FullAdder U1 (.A(A[1]), .B(B[1]), .C_(w_carry1), .S(w_sum[1]), .C(w_carry2));
  assign_1_B_FullAdder U2 (.A(A[2]), .B(B[2]), .C_(w_carry2), .S(w_sum[2]), .C(w_carry3));
  assign_1_B_FullAdder U3 (.A(A[3]), .B(B[3]), .C_(w_carry3), .S(w_sum[3]), .C(Cout));

  // 결과 출력
  assign Sum = w_sum;
  
endmodule