return 
{
  TestType =  "XYTest",
  Channel =  "facebook",
  BinType =  "anonymous",
  description =  "some bogus description",
  Creator =  "joe",
  name =  "Test1",
  Variants =  { {
    name =  "Control",
    percentage =  "50",
    url =  "www.yahoo.com"
  },
  {
    name =  "Variant_A",
    percentage =  "30",
    url =  "www.yahoo.com"
    --[[ Duplicate URL]]--
  },
  {
    name =  "Variant_B",
    percentage =  "20",
    url =  "www.cnn.com"
  }, }
}
