#!/usr/bin/env python3
"""
UEVR Link Validation and Update Script

This script validates and updates cross-references after translation to ensure
all documentation links remain functional and point to English versions.

Features:
- Link discovery in markdown files
- Link validation and testing
- Automatic link updates
- Broken link reporting
- Batch processing support
"""

import os
import re
import csv
import argparse
from pathlib import Path
from typing import List, Dict, Set, Tuple
import urllib.parse
import urllib.request
from urllib.error import URLError, HTTPError

class LinkValidator:
    """Validates and updates links in UEVR documentation."""
    
    def __init__(self, project_root: str):
        self.project_root = Path(project_root)
        self.links = []
        self.broken_links = []
        self.updated_links = []
        
    def discover_links(self) -> None:
        """Discover all links in markdown files."""
        print("Discovering links in markdown files...")
        
        for file_path in self.project_root.rglob('*.md'):
            if 'translation-workspace' in str(file_path):
                continue
                
            try:
                with open(file_path, 'r', encoding='utf-8', errors='ignore') as f:
                    content = f.read()
                    lines = content.split('\n')
                    
                    for line_num, line in enumerate(lines, 1):
                        # Find markdown links [text](url)
                        markdown_links = re.findall(r'\[([^\]]+)\]\(([^)]+)\)', line)
                        for link_text, link_url in markdown_links:
                            self.links.append({
                                'file_path': str(file_path.relative_to(self.project_root)),
                                'line_number': line_num,
                                'link_text': link_text,
                                'link_url': link_url,
                                'link_type': 'markdown',
                                'context': self._extract_context(lines, line_num)
                            })
                        
                        # Find reference links [text][ref] and [ref]: url
                        ref_links = re.findall(r'\[([^\]]+)\]\[([^\]]+)\]', line)
                        for link_text, ref in ref_links:
                            self.links.append({
                                'file_path': str(file_path.relative_to(self.project_root)),
                                'line_number': line_num,
                                'link_text': link_text,
                                'link_url': f'[REF:{ref}]',
                                'link_type': 'reference',
                                'context': self._extract_context(lines, line_num)
                            })
                        
                        # Find reference definitions [ref]: url
                        ref_defs = re.findall(r'^\[([^\]]+)\]:\s*(.+)$', line)
                        for ref, url in ref_defs:
                            self.links.append({
                                'file_path': str(file_path.relative_to(self.project_root)),
                                'line_number': line_num,
                                'link_text': f'[REF:{ref}]',
                                'link_url': url.strip(),
                                'link_type': 'reference_def',
                                'context': self._extract_context(lines, line_num)
                            })
                        
                        # Find file references (common patterns)
                        file_refs = re.findall(r'`([^`]+\.(?:md|txt|cpp|hpp|h|c|ps1|bat|py))`', line)
                        for file_ref in file_refs:
                            self.links.append({
                                'file_path': str(file_path.relative_to(self.project_root)),
                                'line_number': line_num,
                                'link_text': file_ref,
                                'link_url': file_ref,
                                'link_type': 'file_reference',
                                'context': self._extract_context(lines, line_num)
                            })
                            
            except Exception as e:
                print(f"Error reading {file_path}: {e}")
        
        print(f"Discovered {len(self.links)} links")
    
    def _extract_context(self, lines: List[str], line_num: int, context_lines: int = 1) -> str:
        """Extract context around a line."""
        start = max(0, line_num - context_lines - 1)
        end = min(len(lines), line_num + context_lines)
        context_lines_list = lines[start:end]
        return '\n'.join(context_lines_list)
    
    def validate_links(self) -> None:
        """Validate all discovered links."""
        print("Validating links...")
        
        for link in self.links:
            if link['link_type'] == 'markdown':
                self._validate_markdown_link(link)
            elif link['link_type'] == 'file_reference':
                self._validate_file_reference(link)
            elif link['link_type'] == 'reference_def':
                self._validate_reference_definition(link)
    
    def _validate_markdown_link(self, link: Dict) -> None:
        """Validate a markdown link."""
        url = link['link_url']
        
        # Skip external URLs for now
        if url.startswith(('http://', 'https://', 'mailto:', '#')):
            return
        
        # Check if it's a relative file link
        if url.startswith('./') or not url.startswith('/'):
            # Resolve relative path
            link_file = self.project_root / link['file_path']
            link_dir = link_file.parent
            target_path = link_dir / url
            
            if not target_path.exists():
                self.broken_links.append({
                    **link,
                    'issue': 'File not found',
                    'resolved_path': str(target_path)
                })
            else:
                # Check if target needs translation
                if self._needs_translation(target_path):
                    self.broken_links.append({
                        **link,
                        'issue': 'Target needs translation',
                        'resolved_path': str(target_path)
                    })
    
    def _validate_file_reference(self, link: Dict) -> None:
        """Validate a file reference."""
        file_path = link['link_url']
        
        # Resolve relative path
        link_file = self.project_root / link['file_path']
        link_dir = link_file.parent
        target_path = link_dir / file_path
        
        if not target_path.exists():
            self.broken_links.append({
                **link,
                'issue': 'File not found',
                'resolved_path': str(target_path)
            })
        else:
            # Check if target needs translation
            if self._needs_translation(target_path):
                self.broken_links.append({
                    **link,
                    'issue': 'Target needs translation',
                    'resolved_path': str(target_path)
                })
    
    def _validate_reference_definition(self, link: Dict) -> None:
        """Validate a reference definition."""
        url = link['link_url']
        
        # Skip external URLs
        if url.startswith(('http://', 'https://', 'mailto:')):
            return
        
        # Check if it's a relative file link
        if url.startswith('./') or not url.startswith('/'):
            link_file = self.project_root / link['file_path']
            link_dir = link_file.parent
            target_path = link_dir / url
            
            if not target_path.exists():
                self.broken_links.append({
                    **link,
                    'issue': 'File not found',
                    'resolved_path': str(target_path)
                })
    
    def _needs_translation(self, file_path: Path) -> bool:
        """Check if a file needs translation to English."""
        if not file_path.exists():
            return False
        
        try:
            with open(file_path, 'r', encoding='utf-8', errors='ignore') as f:
                content = f.read()
                # Check for Danish characters
                if re.search(r'[æøåÆØÅ]', content):
                    return True
                # Check for common Danish words
                danish_indicators = ['analyser', 'implementering', 'funktionalitet', 'komponenter']
                for indicator in danish_indicators:
                    if indicator in content.lower():
                        return True
        except:
            pass
        
        return False
    
    def update_links(self) -> None:
        """Update links to point to English versions."""
        print("Updating links to English versions...")
        
        for link in self.links:
            if link['link_type'] in ['markdown', 'file_reference']:
                updated_url = self._get_english_version_url(link['link_url'])
                if updated_url != link['link_url']:
                    self.updated_links.append({
                        **link,
                        'old_url': link['link_url'],
                        'new_url': updated_url
                    })
    
    def _get_english_version_url(self, url: str) -> str:
        """Get English version URL for a given link."""
        if not url or url.startswith(('http://', 'https://', 'mailto:', '#')):
            return url
        
        # Check if English version exists
        if url.endswith('.md'):
            # Look for .en.md version
            base_path = Path(url)
            english_path = base_path.parent / f"{base_path.stem}.en{base_path.suffix}"
            if english_path.exists():
                return str(english_path)
        
        return url
    
    def generate_reports(self, output_dir: str) -> None:
        """Generate validation and update reports."""
        output_path = Path(output_dir)
        output_path.mkdir(exist_ok=True)
        
        # Broken links report
        if self.broken_links:
            broken_links_file = output_path / 'broken_links_report.csv'
            with open(broken_links_file, 'w', newline='', encoding='utf-8') as csvfile:
                fieldnames = [
                    'File Path', 'Line Number', 'Link Text', 'Link URL',
                    'Link Type', 'Issue', 'Resolved Path', 'Context'
                ]
                
                writer = csv.DictWriter(csvfile, fieldnames=fieldnames)
                writer.writeheader()
                
                for link in self.broken_links:
                    writer.writerow({
                        'File Path': link['file_path'],
                        'Line Number': link['line_number'],
                        'Link Text': link['link_text'],
                        'Link URL': link['link_url'],
                        'Link Type': link['link_type'],
                        'Issue': link['issue'],
                        'Resolved Path': link.get('resolved_path', ''),
                        'Context': link['context']
                    })
            
            print(f"Broken links report: {broken_links_file}")
        
        # Updated links report
        if self.updated_links:
            updated_links_file = output_path / 'updated_links_report.csv'
            with open(updated_links_file, 'w', newline='', encoding='utf-8') as csvfile:
                fieldnames = [
                    'File Path', 'Line Number', 'Link Text', 'Old URL',
                    'New URL', 'Link Type', 'Context'
                ]
                
                writer = csv.DictWriter(csvfile, fieldnames=fieldnames)
                writer.writeheader()
                
                for link in self.updated_links:
                    writer.writerow({
                        'File Path': link['file_path'],
                        'Line Number': link['line_number'],
                        'Link Text': link['link_text'],
                        'Old URL': link['old_url'],
                        'New URL': link['new_url'],
                        'Link Type': link['link_type'],
                        'Context': link['context']
                    })
            
            print(f"Updated links report: {updated_links_file}")
        
        # Summary report
        summary_file = output_path / 'link_validation_summary.md'
        with open(summary_file, 'w', encoding='utf-8') as f:
            f.write("# Link Validation Summary\n\n")
            f.write(f"**Total Links Discovered**: {len(self.links)}\n")
            f.write(f"**Broken Links**: {len(self.broken_links)}\n")
            f.write(f"**Updated Links**: {len(self.updated_links)}\n\n")
            
            f.write("## Link Types\n\n")
            link_types = {}
            for link in self.links:
                link_type = link['link_type']
                link_types[link_type] = link_types.get(link_type, 0) + 1
            
            for link_type, count in sorted(link_types.items()):
                f.write(f"- **{link_type}**: {count}\n")
            
            f.write("\n## Issues Found\n\n")
            if self.broken_links:
                f.write("### Broken Links\n\n")
                for link in self.broken_links:
                    f.write(f"- **{link['file_path']}:{link['line_number']}** - {link['issue']}\n")
                    f.write(f"  - Link: `{link['link_text']}` → `{link['link_url']}`\n")
                    f.write(f"  - Issue: {link['issue']}\n\n")
            else:
                f.write("✅ No broken links found!\n\n")
            
            f.write("## Recommendations\n\n")
            if self.broken_links:
                f.write("1. **Fix broken links** before completing translation\n")
                f.write("2. **Update file references** to point to English versions\n")
                f.write("3. **Validate all cross-references** after translation\n")
            else:
                f.write("1. **Proceed with translation** - all links are valid\n")
                f.write("2. **Update links** to point to English versions\n")
                f.write("3. **Run validation again** after translation completion\n")
        
        print(f"Summary report: {summary_file}")
    
    def generate_summary(self) -> None:
        """Generate summary of link validation."""
        total_links = len(self.links)
        broken_links = len(self.broken_links)
        updated_links = len(self.updated_links)
        
        link_types = {}
        for link in self.links:
            link_type = link['link_type']
            link_types[link_type] = link_types.get(link_type, 0) + 1
        
        print("\n" + "="*60)
        print("LINK VALIDATION SUMMARY")
        print("="*60)
        print(f"Total Links: {total_links}")
        print(f"Broken Links: {broken_links}")
        print(f"Updated Links: {updated_links}")
        print(f"Success Rate: {((total_links - broken_links) / total_links * 100):.1f}%")
        print("\nLinks by Type:")
        for link_type, count in sorted(link_types.items()):
            print(f"  {link_type}: {count}")
        print("="*60)

def main():
    parser = argparse.ArgumentParser(description='Validate and update links in UEVR documentation')
    parser.add_argument('--project-root', default='.', help='Project root directory')
    parser.add_argument('--output-dir', default='link_reports', help='Output directory for reports')
    
    args = parser.parse_args()
    
    validator = LinkValidator(args.project_root)
    validator.discover_links()
    validator.validate_links()
    validator.update_links()
    validator.generate_reports(args.output_dir)
    validator.generate_summary()

if __name__ == '__main__':
    main()
