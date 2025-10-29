"""
Claude (Anthropic) AI Provider
"""

import asyncio
from typing import Optional
from .base import BaseProvider

try:
    import anthropic
except ImportError:
    anthropic = None


class ClaudeProvider(BaseProvider):
    """Claude AI provider using Anthropic's API"""
    
    def __init__(self, api_key: str, model: str = "claude-3-sonnet-20240229", **kwargs):
        super().__init__(api_key, model, **kwargs)
        self.client = None
        if anthropic:
            self.client = anthropic.Anthropic(api_key=api_key)
    
    @property
    def provider_name(self) -> str:
        return "Claude"
    
    async def generate_response(self, prompt: str, **kwargs) -> str:
        """Generate response using Claude"""
        if not self.client:
            raise RuntimeError("Anthropic client not available. Install with: pip install anthropic")
        
        try:
            # Claude expects messages format
            message = await asyncio.to_thread(
                self.client.messages.create,
                model=self.model,
                max_tokens=kwargs.get('max_tokens', 4000),
                temperature=kwargs.get('temperature', 0.7),
                messages=[
                    {"role": "user", "content": prompt}
                ]
            )
            return message.content[0].text
        except Exception as e:
            return f"Error from Claude: {str(e)}"
    
    def validate_connection(self) -> bool:
        """Validate Claude API connection"""
        if not self.client:
            return False
        
        try:
            # Try a simple API call to validate the key
            self.client.messages.create(
                model=self.model,
                max_tokens=10,
                messages=[{"role": "user", "content": "Hi"}]
            )
            return True
        except Exception:
            return False
