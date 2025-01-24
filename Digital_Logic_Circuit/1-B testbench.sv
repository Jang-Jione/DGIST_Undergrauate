module assign_1_B_4bParallelAdder_tb;

  reg [3:0] A, B;

  wire [3:0] Sum;
  wire [3:0] Cout;

  assign_1_B_4bParallelAdder U0 (
    .A(A),
    .B(B),
    .Sum(Sum),
    .Cout(Cout)
  );

  initial begin
    for (int i = 0; i < 8; i=i+1) begin
      A = $random;
      B = $random;
      #10; 
      $display("Test Case %0d: A=%b, B=%b, Sum=%b, Cout=%b", i+1, A, B, Sum, Cout);
    end

    $finish;
  end

endmodule