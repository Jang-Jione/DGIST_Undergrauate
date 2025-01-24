module flash_adc_decoder (
  input wire [3:0] COMP,
  output wire [1:0] B
);

  wire W1, W2, W3;
  
  nand G1 (W1, COMP[3], ~COMP[2]);
  nand G2 (W2, COMP[2], ~COMP[1]);
  nand G3 (W3, COMP[1], ~COMP[0]);
  
  nand G4 (B[1], W1, W2);
  nand G5 (B[0], W1, W3);
  
endmodule