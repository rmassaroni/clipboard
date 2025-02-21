from transformers import GPT2LMHeadModel, GPT2Tokenizer

# Load model and tokenizer
model = GPT2LMHeadModel.from_pretrained("gpt2")
tokenizer = GPT2Tokenizer.from_pretrained("gpt2")

# Generate text based on a prompt
# prompt = "Once upon a time"
prompt = ""
inputs = tokenizer(prompt, return_tensors="pt")
outputs = model.generate(inputs['input_ids'], max_length=100)

# Decode and print the output
generated_text = tokenizer.decode(outputs[0], skip_special_tokens=True)
print(generated_text)

