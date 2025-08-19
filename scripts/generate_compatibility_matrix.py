#!/usr/bin/env python3
"""
UEVR Compatibility Matrix Generator

Automatically generates the compatibility matrix from profiles/*/adapter.json files.
This script reads all game profiles and creates a comprehensive compatibility report.

Usage:
    python scripts/generate_compatibility_matrix.py [--output OUTPUT_FILE] [--format FORMAT]

Formats:
    - markdown (default)
    - html
    - json
    - csv
"""

import json
import os
import sys
import argparse
from pathlib import Path
from typing import Dict, List, Any, Optional
from datetime import datetime
import yaml

class CompatibilityMatrixGenerator:
    def __init__(self, profiles_dir: str = "profiles"):
        self.profiles_dir = Path(profiles_dir)
        self.compatibility_data = {}
        self.engine_stats = {}
        self.tier_stats = {}
        
    def load_profile(self, profile_path: Path) -> Optional[Dict[str, Any]]:
        """Load and validate a single profile file."""
        try:
            with open(profile_path, 'r', encoding='utf-8') as f:
                profile = json.load(f)
            
            # Validate required fields
            required_fields = ['engine', 'version', 'render', 'camera', 'ui', 'input', 'performance']
            for field in required_fields:
                if field not in profile:
                    print(f"Warning: Missing required field '{field}' in {profile_path}")
                    return None
            
            return profile
        except json.JSONDecodeError as e:
            print(f"Error: Invalid JSON in {profile_path}: {e}")
            return None
        except Exception as e:
            print(f"Error: Failed to load {profile_path}: {e}")
            return None
    
    def scan_profiles(self) -> None:
        """Scan all profile directories and load adapter.json files."""
        if not self.profiles_dir.exists():
            print(f"Error: Profiles directory '{self.profiles_dir}' does not exist")
            return
        
        print(f"Scanning profiles directory: {self.profiles_dir}")
        
        for profile_dir in self.profiles_dir.iterdir():
            if not profile_dir.is_dir() or profile_dir.name.startswith('_'):
                continue
            
            adapter_file = profile_dir / "adapter.json"
            if not adapter_file.exists():
                continue
            
            print(f"Loading profile: {profile_dir.name}")
            profile = self.load_profile(adapter_file)
            if profile:
                self.compatibility_data[profile_dir.name] = profile
                self.update_statistics(profile_dir.name, profile)
    
    def update_statistics(self, game_name: str, profile: Dict[str, Any]) -> None:
        """Update engine and tier statistics."""
        # Engine statistics
        engine = profile.get('engine', 'Unknown')
        if engine not in self.engine_stats:
            self.engine_stats[engine] = []
        self.engine_stats[engine].append(game_name)
        
        # Tier statistics
        tier = profile.get('compatibility_tier', 1)
        if tier not in self.tier_stats:
            self.tier_stats[tier] = []
        self.tier_stats[tier].append(game_name)
    
    def generate_markdown(self) -> str:
        """Generate compatibility matrix in Markdown format."""
        md = []
        md.append("# UEVR Compatibility Matrix")
        md.append("")
        md.append(f"*Generated on: {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}*")
        md.append(f"*Total Games: {len(self.compatibility_data)}*")
        md.append("")
        
        # Summary statistics
        md.append("## Summary Statistics")
        md.append("")
        
        # Engine distribution
        md.append("### Engine Distribution")
        md.append("")
        for engine, games in sorted(self.engine_stats.items()):
            md.append(f"- **{engine}**: {len(games)} games")
        md.append("")
        
        # Tier distribution
        md.append("### Compatibility Tiers")
        md.append("")
        tier_descriptions = {
            0: "Not detected/blocked (no hooks)",
            1: "Boot + basic camera pose (no UI, flat view)",
            2: "Stereo + head-tracked camera + FOV control",
            3: "Input remap + HUD transforms + basic 6DoF",
            4: "Full VR integration (stereo rendering, hands, UI reprojection)",
            5: "Enhanced features (DLSS/FSR path, neural interpolation, reprojection)"
        }
        
        for tier in sorted(self.tier_stats.keys()):
            games = self.tier_stats[tier]
            description = tier_descriptions.get(tier, f"Tier {tier}")
            md.append(f"- **Tier {tier}**: {len(games)} games - {description}")
        md.append("")
        
        # Detailed compatibility matrix
        md.append("## Detailed Compatibility Matrix")
        md.append("")
        md.append("| Game | Engine | Tier | Features | Status | Notes |")
        md.append("|------|--------|------|----------|--------|-------|")
        
        for game_name, profile in sorted(self.compatibility_data.items()):
            engine = profile.get('engine', 'Unknown')
            tier = profile.get('compatibility_tier', 1)
            
            # VR features
            vr_features = profile.get('vr_features', {})
            features = []
            if vr_features.get('stereo_rendering'):
                features.append("Stereo")
            if vr_features.get('motion_controllers'):
                features.append("Controllers")
            if vr_features.get('haptic_feedback'):
                features.append("Haptic")
            if vr_features.get('dynamic_fov'):
                features.append("Dynamic FOV")
            if vr_features.get('neural_upscaling'):
                features.append("Neural")
            if vr_features.get('ray_tracing'):
                features.append("RT")
            
            features_str = ", ".join(features) if features else "Basic"
            
            # Status indicator
            if tier >= 4:
                status = "✅"
            elif tier >= 2:
                status = "🟡"
            else:
                status = "❌"
            
            # Notes
            notes = profile.get('notes', [])
            notes_str = "; ".join(notes[:2]) if notes else ""
            if len(notes) > 2:
                notes_str += f" (+{len(notes)-2} more)"
            
            md.append(f"| {game_name} | {engine} | {tier} | {features_str} | {status} | {notes_str} |")
        
        md.append("")
        
        # Engine-specific details
        md.append("## Engine-Specific Details")
        md.append("")
        
        for engine, games in sorted(self.engine_stats.items()):
            md.append(f"### {engine}")
            md.append("")
            md.append(f"**Supported Games**: {len(games)}")
            md.append("")
            
            # Game list
            for game in sorted(games):
                profile = self.compatibility_data[game]
                tier = profile.get('compatibility_tier', 1)
                md.append(f"- {game} (Tier {tier})")
            
            md.append("")
        
        return "\n".join(md)
    
    def generate_html(self) -> str:
        """Generate compatibility matrix in HTML format."""
        html = []
        html.append("<!DOCTYPE html>")
        html.append("<html lang='en'>")
        html.append("<head>")
        html.append("    <meta charset='UTF-8'>")
        html.append("    <meta name='viewport' content='width=device-width, initial-scale=1.0'>")
        html.append("    <title>UEVR Compatibility Matrix</title>")
        html.append("    <style>")
        html.append("        body { font-family: Arial, sans-serif; margin: 20px; }")
        html.append("        table { border-collapse: collapse; width: 100%; margin: 20px 0; }")
        html.append("        th, td { border: 1px solid #ddd; padding: 8px; text-align: left; }")
        html.append("        th { background-color: #f2f2f2; }")
        html.append("        .tier-0 { background-color: #ffebee; }")
        html.append("        .tier-1 { background-color: #fff3e0; }")
        html.append("        .tier-2 { background-color: #e8f5e8; }")
        html.append("        .tier-3 { background-color: #e3f2fd; }")
        html.append("        .tier-4 { background-color: #f3e5f5; }")
        html.append("        .tier-5 { background-color: #e8f5e8; }")
        html.append("    </style>")
        html.append("</head>")
        html.append("<body>")
        
        # Convert markdown to HTML
        md_content = self.generate_markdown()
        
        # Simple markdown to HTML conversion
        html_content = md_content.replace("# ", "<h1>").replace("\n# ", "</h1>\n<h1>")
        html_content = html_content.replace("## ", "<h2>").replace("\n## ", "</h2>\n<h2>")
        html_content = html_content.replace("### ", "<h3>").replace("\n### ", "</h3>\n<h3>")
        html_content = html_content.replace("**", "<strong>").replace("**", "</strong>")
        html_content = html_content.replace("*", "<em>").replace("*", "</em>")
        
        # Convert table
        lines = html_content.split('\n')
        in_table = False
        html_lines = []
        
        for line in lines:
            if line.startswith('|'):
                if not in_table:
                    html_lines.append('<table>')
                    in_table = True
                
                # Parse table row
                cells = [cell.strip() for cell in line.split('|')[1:-1]]
                if line.startswith('|------'):
                    # Header separator
                    continue
                
                if line.startswith('| Game |'):
                    # Header row
                    html_lines.append('<tr>')
                    for cell in cells:
                        html_lines.append(f'<th>{cell}</th>')
                    html_lines.append('</tr>')
                else:
                    # Data row
                    tier = 1
                    if 'Tier' in line:
                        for cell in cells:
                            if 'Tier' in cell:
                                try:
                                    tier = int(cell.split()[-1])
                                    break
                                except:
                                    pass
                    
                    html_lines.append(f'<tr class="tier-{tier}">')
                    for cell in cells:
                        html_lines.append(f'<td>{cell}</td>')
                    html_lines.append('</tr>')
            else:
                if in_table:
                    html_lines.append('</table>')
                    in_table = False
                html_lines.append(line)
        
        if in_table:
            html_lines.append('</table>')
        
        html.extend(html_lines)
        html.append("</body>")
        html.append("</html>")
        
        return "\n".join(html)
    
    def generate_json(self) -> str:
        """Generate compatibility matrix in JSON format."""
        output = {
            "generated": datetime.now().isoformat(),
            "total_games": len(self.compatibility_data),
            "engine_distribution": self.engine_stats,
            "tier_distribution": self.tier_stats,
            "compatibility_matrix": self.compatibility_data
        }
        return json.dumps(output, indent=2)
    
    def generate_csv(self) -> str:
        """Generate compatibility matrix in CSV format."""
        csv = []
        csv.append("Game,Engine,Tier,Features,Status,Notes")
        
        for game_name, profile in sorted(self.compatibility_data.items()):
            engine = profile.get('engine', 'Unknown')
            tier = profile.get('compatibility_tier', 1)
            
            # VR features
            vr_features = profile.get('vr_features', {})
            features = []
            if vr_features.get('stereo_rendering'):
                features.append("Stereo")
            if vr_features.get('motion_controllers'):
                features.append("Controllers")
            if vr_features.get('haptic_feedback'):
                features.append("Haptic")
            if vr_features.get('dynamic_fov'):
                features.append("Dynamic FOV")
            if vr_features.get('neural_upscaling'):
                features.append("Neural")
            if vr_features.get('ray_tracing'):
                features.append("RT")
            
            features_str = "; ".join(features) if features else "Basic"
            
            # Status
            if tier >= 4:
                status = "Complete"
            elif tier >= 2:
                status = "In Progress"
            else:
                status = "Blocked"
            
            # Notes
            notes = profile.get('notes', [])
            notes_str = "; ".join(notes) if notes else ""
            
            # Escape CSV fields
            game_name_escaped = f'"{game_name}"' if ',' in game_name else game_name
            notes_escaped = f'"{notes_str}"' if ',' in notes_str else notes_str
            
            csv.append(f"{game_name_escaped},{engine},{tier},{features_str},{status},{notes_escaped}")
        
        return "\n".join(csv)
    
    def generate_report(self, output_format: str = "markdown", output_file: Optional[str] = None) -> str:
        """Generate compatibility matrix report in the specified format."""
        if output_format == "markdown":
            content = self.generate_markdown()
        elif output_format == "html":
            content = self.generate_html()
        elif output_format == "json":
            content = self.generate_json()
        elif output_format == "csv":
            content = self.generate_csv()
        else:
            raise ValueError(f"Unsupported format: {output_format}")
        
        # Write to file if specified
        if output_file:
            output_path = Path(output_file)
            output_path.parent.mkdir(parents=True, exist_ok=True)
            
            with open(output_path, 'w', encoding='utf-8') as f:
                f.write(content)
            
            print(f"Compatibility matrix written to: {output_path}")
        
        return content

def main():
    parser = argparse.ArgumentParser(description="Generate UEVR compatibility matrix")
    parser.add_argument("--output", "-o", help="Output file path")
    parser.add_argument("--format", "-f", choices=["markdown", "html", "json", "csv"], 
                       default="markdown", help="Output format")
    parser.add_argument("--profiles", "-p", default="profiles", help="Profiles directory path")
    
    args = parser.parse_args()
    
    # Generate compatibility matrix
    generator = CompatibilityMatrixGenerator(args.profiles)
    generator.scan_profiles()
    
    if not generator.compatibility_data:
        print("No profiles found. Make sure the profiles directory contains valid adapter.json files.")
        sys.exit(1)
    
    # Generate report
    try:
        content = generator.generate_report(args.format, args.output)
        
        if not args.output:
            print(content)
        
        print(f"Generated compatibility matrix for {len(generator.compatibility_data)} games")
        print(f"Format: {args.format}")
        
    except Exception as e:
        print(f"Error generating report: {e}")
        sys.exit(1)

if __name__ == "__main__":
    main()


